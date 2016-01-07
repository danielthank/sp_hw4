#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <string.h>

#define MAXN 50000000
#define min(a, b) ((a)<(b)?(a):(b))

int n, array[MAXN];
pthread_t tid[MAXN];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct Segment {
    int start, end;
};

struct Merge {
    int astart, bstart, bend;
};

void *presort(void *arg) {
    Segment *seg = (Segment*)arg;
    int start = seg->start, end = seg->end;
    pthread_mutex_lock(&lock);
    printf("Handling elements:\n");
    printf("%d", array[start]);
    for (int i=start+1; i<end; i++)
        printf(" %d", array[i]);
    printf("\nSorted %d elements\n", end-start);
    pthread_mutex_unlock(&lock);
    std::sort(array+start, array+end);
    return (void*) 0;
}

void *merge(void *arg) {
    Merge *data= (Merge*)arg;
    int x = data->astart, y = data->bstart, z = data->bend;
    int xx = 0, yy = 0, now = x;
    int alen = y-x, blen = z-y;
    int *a = new int[alen+blen];
    int *b = a+alen;
    int dup = 0;
    memcpy(a, array+x, (alen+blen)*sizeof(int));
    while(xx < alen && yy < blen) {
        if (a[xx] <= b[yy]) {
            if (a[xx] == b[yy]) dup++;
            array[now++] = a[xx++];
        }
        else {
            array[now++] = b[yy++];
        }
    }
    while (xx < alen) array[now++] = a[xx++];
    while (yy < blen) array[now++] = b[yy++];
    pthread_mutex_lock(&lock);
    printf("Handling elements:\n");
    printf("%d", a[0]);
    for (int i=1; i<alen+blen; i++)
        printf(" %d", a[i]);
    printf("\n");
    printf("Merged %d and %d elements with %d duplicates.\n", alen, blen, dup);
    pthread_mutex_unlock(&lock);
    delete[] a;
    return (void*)0;
}

int main(int argc, char *argv[]) {
    scanf("%d", &n);
    for (int i=0; i<n; i++) scanf("%d", array+i);
    int cut = atoi(argv[1]);
    Segment *seg = new Segment[n/cut+1];
    int i;
    for (i=0; i*cut<n; i++) {
        seg[i] = (Segment){i*+cut, min((i+1)*cut, n)};
        pthread_create(tid+i, NULL, presort, (void*)(seg+i));
    }
    while(i!=0) pthread_join(tid[--i], NULL);
    delete[] seg;
    while (cut < n) {
        Merge *mrg = new Merge[n/(cut*2)+1];
        int i;
        for (i=0; (2*i+1)*cut < n; i++) {
            mrg[i] = (Merge){2*i*cut, (2*i+1)*cut, min(n, (2*i+2)*cut)};
            pthread_create(tid+i, NULL, merge, (void*)(mrg+i));
        }
        while(i!=0) pthread_join(tid[--i], NULL);
        cut *= 2;
    }
    printf("%d", array[0]);
    for (i=1; i<n; i++)
        printf(" %d", array[i]);
    printf("\n");
    return 0;
}
