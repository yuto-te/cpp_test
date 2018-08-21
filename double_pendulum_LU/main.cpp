/*
2重振り子
LU分解で連立方程式を解く
RK4
*/

#include<iostream>
#include<cmath>
#include<fstream>

#include<boost/multiprecision/cpp_dec_float.hpp>
#include<Eigen/Core>
#include<Eigen/LU>

namespace mp = boost::multiprecision;
using multiFloat = mp::cpp_dec_float_100;

using std::sin;
using std::cos;

// 重力加速度
const multiFloat g = 9.80665;

// 時間パラメータ
const multiFloat tlim = 100.;
const multiFloat dt = 0.01;

// 振り子のパラメータ
const multiFloat m1 = 5.;
const multiFloat m2 = 2.;
const multiFloat l1 = 0.5;
const multiFloat l2 = 1.;

// 運動方程式
Eigen::Matrix<multiFloat, 4, 1> updateCondition(const Eigen::Matrix<multiFloat, 4, 1> condition){
    multiFloat theta1 = condition(0,0);
    multiFloat theta2 = condition(1,0);
    multiFloat dtheta1 = condition(2,0);
    multiFloat dtheta2 = condition(3,0);

    Eigen::Matrix<multiFloat, 2, 2> A;
    A << (m1 + m2)*l1, m2*l2*cos(theta1 - theta2),
         l1*l2*cos(theta1 - theta2), l2*l2;
    Eigen::Matrix<multiFloat, 2, 1> b;
    b << -m2*l2*dtheta2*dtheta2*sin(theta1 - theta2) - (m1 + m2)*g*sin(theta1),
         l1*l2*dtheta1*dtheta1*sin(theta1 - theta2) - g*l2*sin(theta2);
    Eigen::FullPivLU<Eigen::Matrix<multiFloat, 2, 2> > LU(A);
    Eigen::Matrix<multiFloat, 2, 1> x;
    x = LU.solve(b);

    return Eigen::Matrix<multiFloat, 4, 1>{
        dtheta1,
        dtheta2,
        x(0),
        x(1)
    };
}

Eigen::Matrix<multiFloat, 4, 1> initialCondition(){
    multiFloat theta1 = 4.*std::atan(1.) * 2. / 2.;
    multiFloat theta2 = 4.*std::atan(1.) * 0. / 2.;
    multiFloat dtheta1 = 0.;
    multiFloat dtheta2 = 0.001;

    return Eigen::Matrix<multiFloat, 4, 1>{
        theta1,
        theta2,
        dtheta1,
        dtheta2
    };
}

multiFloat potentialEnergy(const Eigen::Matrix<multiFloat, 4, 1> x){
    multiFloat theta1 = x(0,0);
    multiFloat theta2 = x(1,0);
    return -m1*g*cos(theta1) - m2*g*(l1*cos(theta1) + l2*cos(theta2));
}

multiFloat kineticEnergy(const Eigen::Matrix<multiFloat, 4, 1> x){
    multiFloat theta1 = x(0,0);
    multiFloat theta2 = x(1,0);
    multiFloat dtheta1 = x(2,0);
    multiFloat dtheta2 = x(3,0);

    return 0.5*m1*l1*l1*dtheta1*dtheta1 + 0.5*m2*(l1*l1*dtheta1*dtheta1 + l2*l2*dtheta2*dtheta2 + 2*l1*l2*dtheta1*dtheta2*cos(theta1 - theta2));
}

int main()
{
    Eigen::Matrix<multiFloat, 4, 1> x = initialCondition();
    Eigen::Matrix<multiFloat, 4, 1> k1, k2, k3, k4;
    multiFloat t = 0., KE, PE;

    // 出力ファイル
    std::fstream data;
    data.open("data.csv", std::ios::out);

    FILE *gp = popen("gnuplot","w");
    fprintf(gp, "set nokey\n");
    fprintf(gp, "set size square\n");
    fprintf(gp, "set term gif animate optimize delay 1 size 360,360\n");
    fprintf(gp, "set output 'movie.gif'\n");
    fprintf(gp, "set xr [%f:%f]\n", static_cast<float>(-l1 - l2), static_cast<float>(l1 + l2));
    fprintf(gp, "set yr [%f:%f]\n", static_cast<float>(-l1 - l2), static_cast<float>(l1 + l2));

    for(int i = 0; t < tlim; ++i){
        KE = kineticEnergy(x);
        PE = potentialEnergy(x);
        std::cout << i*dt << " " << KE + PE << std::endl; // エネルギーが保存されているか確認

        if (i%10==0){
            // gnuplot
            fprintf(gp, "plot '-' w lp lw 3\n");
            fprintf(gp, "0.0 0.0\n");
            fprintf(gp, "%f %f\n", static_cast<float>(l1*sin(x(0,0))), static_cast<float>(-l1*cos(x(0,0))));
            fprintf(gp, "%f %f\n", static_cast<float>(l1*sin(x(0,0)) + l2*sin(x(1,0))), static_cast<float>(-l1*cos(x(0,0)) - l2*cos(x(1,0))));
            fprintf(gp, "e\n");
        }

        // csv
        data << x(0,0) << "," << x(1,0) << "," << x(2,0) << "," << x(3,0) << std::endl;

        // RK4
        k1 = updateCondition(x);
        k2 = updateCondition(x + dt/2*k1);
        k3 = updateCondition(x + dt/2*k2);
        k4 = updateCondition(x + dt*k3);

        x += dt/6*(k1 + 2*k2 + 2*k3 + k4);
        t += dt;
    }
    pclose(gp);
    data.close();
}
