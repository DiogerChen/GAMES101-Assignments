/*
    给定一个点P=(2,1), 将该点绕原点先逆时针旋转45°, 再平移(1,2),
    计算出变换后点的坐标(要求用齐次坐标进行计算)。
*/
#include <cmath>
#include <iostream>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

using namespace std;

int main(int argc, char *argv[])
{
    const float deg2rad = acos(-1) / 180.0f;

    Eigen::Vector3f P_point(2.0f, 1.0f, 1.0f);
    float rotation_deg = 45.0f;
    float rotation_red = rotation_deg * deg2rad;

    float translation_x = 1;
    float translation_y = 2;
    
    Eigen::Matrix3f rotation_matrix, translation_matrix;
    rotation_matrix <<  cos(rotation_red), -sin(rotation_red), 0.0f,
                        sin(rotation_red), cos(rotation_red), 0.0f,
                        0.0f, 0.0f, 1.0f;
    translation_matrix <<   1.0f, 0.0f, translation_x,
                            0.0f, 1.0f, translation_y,
                            0.0f, 0.0f, 1.0f;
    Eigen::Vector3f P_point_new = translation_matrix * rotation_matrix * P_point;
    cout << P_point_new << endl;

    return 0;
}