#ifndef ECVRPTW_UTILS_H
#define ECVRPTW_UTILS_H

template<class T>
struct ptr_less {
    bool operator() (const T* const A, const T* const B) const{
        return *A < *B;
    }
};

#endif //ECVRPTW_UTILS_H
