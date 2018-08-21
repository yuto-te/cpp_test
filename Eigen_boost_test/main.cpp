/*
Eigenとboost/multiprecision/cpp_dec_float.hppの組み合わせを試す
*/

#include<bits/stdc++.h>
#include<Eigen/Core>
#include<Eigen/LU>
#include<boost/multiprecision/cpp_dec_float.hpp>

namespace mp = boost::multiprecision;
using multiFloat = mp::cpp_dec_float_100;

void func(){
    Eigen::Matrix<multiFloat, 2, 2> A;
    A << 1., 2., 3., 4.;
    Eigen::Matrix<multiFloat, 2, 1> b;
    b << 1., 2.;
    Eigen::FullPivLU<Eigen::Matrix<multiFloat, 2, 2> > LU(A);
    Eigen::Matrix<multiFloat, 2, 1> x;
    x = LU.solve(b);
    std::cout << A << std::endl;
    std::cout << b << std::endl;
    std::cout << x << std::endl;
    printf("%f", static_cast<float>(x(0)));
}

int main(){
    func();
}