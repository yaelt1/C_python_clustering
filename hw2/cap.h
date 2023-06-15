#ifndef CAP_H_
#define CAP_H_

struct vector *kmeans(int K, int iter, int vec_cords, int num_of_points, struct vector *init_cent, double points[][vec_cords]);

#endif