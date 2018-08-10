/*
2重振り子
RK4
*/

#include<iostream>
#include <cmath>

#include "Eigen/Core"

// 重力加速度
constexpr double g = 9.80665;

// 時間パラメータ
constexpr double tlim = 100.;
constexpr double dt = 0.01;

// 振り子のパラメータ
constexpr double m1 = 5.;
constexpr double m2 = 2.;
constexpr double l1 = 0.5;
constexpr double l2 = 1.;

using std::sin;
using std::cos;

// 運動方程式
Eigen::Matrix<double, 4, 1> updateCondition(const Eigen::Matrix<double, 4, 1> x){
    double theta1 = x(0,0);
    double theta2 = x(1,0);
    double dtheta1 = x(2,0);
    double dtheta2 = x(3,0);

    return Eigen::Matrix<double, 4, 1>{
        dtheta1,
        dtheta2,
        (-m1*g*sin(theta1) - m2*(g*sin(theta1) + l2*dtheta2*dtheta2*sin(theta1 - theta2) + (l1*dtheta1*dtheta1*sin(theta1 - theta2) - g*sin(theta2))*cos(theta1 - theta2)))/(l1*(m1 + m2*sin(theta1 - theta2)*sin(theta1 - theta2))),
        ((m1 + m2)*(l1*dtheta1*dtheta1*sin(theta1 - theta2) - g*sin(theta2) + g*sin(theta1)*cos(theta1-theta2)) + m2*l2*dtheta2*dtheta2*cos(theta1 - theta2)*sin(theta1 - theta2))/(l2*(m1 + m2*sin(theta1 - theta2)*sin(theta1 - theta2)))
    };
}

double potentialEnergy(const Eigen::Matrix<double, 4, 1> x){
    double theta1 = x(0,0);
    double theta2 = x(1,0);
    return -m1*g*cos(theta1) - m2*g*(l1*cos(theta1) + l2*cos(theta2));
}

double kineticEnergy(const Eigen::Matrix<double, 4, 1> x){
    double theta1 = x(0,0);
    double theta2 = x(1,0);
    double dtheta1 = x(2,0);
    double dtheta2 = x(3,0);

    return 0.5*m1*l1*l1*dtheta1*dtheta1 + 0.5*m2*(l1*l1*dtheta1*dtheta1 + l2*l2*dtheta2*dtheta2 + 2*l1*l2*dtheta1*dtheta2*cos(theta1 - theta2));
}

Eigen::Matrix<double, 4, 1> initialCondition(){
    double theta1 = 4.*std::atan(1.) * 2. / 2.;
    double theta2 = 4.*std::atan(1.) * 0. / 2.;
    double dtheta1 = 0.;
    double dtheta2 = 0.001;

    return Eigen::Matrix<double, 4, 1>{
        theta1,
        theta2,
        dtheta1,
        dtheta2
    };
}

int main()
{
    Eigen::Matrix<double, 4, 1> x = initialCondition();
    Eigen::Matrix<double, 4, 1> k1, k2, k3, k4;
    double t = 0., KE, PE;
    FILE *gp = popen("gnuplot","w");
    fprintf(gp, "set nokey\n");
    fprintf(gp, "set size square\n");
    fprintf(gp, "set term gif animate optimize delay 0.1 size 360,360\n");
    fprintf(gp, "set output 'movie.gif'\n");
    fprintf(gp, "set xr [%f:%f]\n", -l1 - l2, l1 + l2);
    fprintf(gp, "set yr [%f:%f]\n", -l1 - l2, l1 + l2);

    for(std::size_t i {}; t < tlim; ++i){
        // KE = kineticEnergy(x);
        // PE = potentialEnergy(x);
        // std::cout << i*dt << " " << KE + PE << std::endl; // エネルギーが保存されているか確認

        // gnuplot
        fprintf(gp, "plot '-' w lp lw 3\n");
        fprintf(gp, "0.0 0.0\n");
        fprintf(gp, "%f %f\n", l1*sin(x(0,0)), -l1*cos(x(0,0)));
        fprintf(gp, "%f %f\n", l1*sin(x(0,0)) + l2*sin(x(1,0)), -l1*cos(x(0,0)) - l2*cos(x(1,0)));
        fprintf(gp, "e\n");
        // fprintf(gp, "set output\n");
        // fflush(gp);

        // RK4
        k1 = updateCondition(x);
        k2 = updateCondition(x + dt/2*k1);
        k3 = updateCondition(x + dt/2*k2);
        k4 = updateCondition(x + dt*k3);

        x += dt/6*(k1 + 2*k2 + 2*k3 + k4);
        t += dt;
    }
    // fprintf(gp, "set out\n");
    // fprintf(gp, "set terminal wxt enhanced\n");
    pclose(gp);
}
