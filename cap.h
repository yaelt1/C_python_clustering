#ifndef CAP_H_
#define CAP_H_

struct vector *kmeans(int K, int iter, int vec_cords, double epsilon, int len, struct vector *init_cent, struct vector *head_vec);

#endif