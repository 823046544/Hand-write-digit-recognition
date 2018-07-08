/*
svm_type –
指定SVM的类型，下面是可能的取值：
CvSVM::C_SVC C类支持向量分类机。 n类分组  (n \geq 2)，允许用异常值惩罚因子C进行不完全分类。
CvSVM::NU_SVC \nu类支持向量分类机。n类似然不完全分类的分类器。参数为 \nu 取代C（其值在区间【0，1】中，nu越大，决策边界越平滑）。
CvSVM::ONE_CLASS 单分类器，所有的训练数据提取自同一个类里，然后SVM建立了一个分界线以分割该类在特征空间中所占区域和其它类在特征空间中所占区域。
CvSVM::EPS_SVR \epsilon类支持向量回归机。训练集中的特征向量和拟合出来的超平面的距离需要小于p。异常值惩罚因子C被采用。
CvSVM::NU_SVR \nu类支持向量回归机。 \nu 代替了 p。

可从 [LibSVM] 获取更多细节。

kernel_type –
SVM的内核类型，下面是可能的取值：
CvSVM::LINEAR 线性内核。没有任何向映射至高维空间，线性区分（或回归）在原始特征空间中被完成，这是最快的选择。K(x_i, x_j) = x_i^T x_j.
CvSVM::POLY 多项式内核： K(x_i, x_j) = (\gamma x_i^T x_j + coef0)^{degree}, \gamma > 0.
CvSVM::RBF 基于径向的函数，对于大多数情况都是一个较好的选择： K(x_i, x_j) = e^{-\gamma ||x_i - x_j||^2}, \gamma > 0.
CvSVM::SIGMOID Sigmoid函数内核：K(x_i, x_j) = \tanh(\gamma x_i^T x_j + coef0).

degree – 内核函数（POLY）的参数degree。

gamma – 内核函数（POLY/ RBF/ SIGMOID）的参数\gamma。

coef0 – 内核函数（POLY/ SIGMOID）的参数coef0。

Cvalue – SVM类型（C_SVC/ EPS_SVR/ NU_SVR）的参数C。

nu – SVM类型（NU_SVC/ ONE_CLASS/ NU_SVR）的参数 \nu。

p – SVM类型（EPS_SVR）的参数 \epsilon。

class_weights – C_SVC中的可选权重，赋给指定的类，乘以C以后变成 class\_weights_i * C。所以这些权重影响不同类别的错误分类惩罚项。权重越大，某一类别的误分类数据的惩罚项就越大。

term_crit – SVM的迭代训练过程的中止条件，解决部分受约束二次最优问题。您可以指定的公差和/或最大迭代次数。

*/


#include "mnist.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/ml.hpp>

#include <string>
#include <iostream>

using namespace std;
using namespace cv;
using namespace cv::ml;

string trainImage = "mnist_dataset/train-images.idx3-ubyte";
string trainLabel = "mnist_dataset/train-labels.idx1-ubyte";
string testImage = "mnist_dataset/t10k-images.idx3-ubyte";
string testLabel = "mnist_dataset/t10k-labels.idx1-ubyte";
//string testImage = "mnist_dataset/train-images.idx3-ubyte";
//string testLabel = "mnist_dataset/train-labels.idx1-ubyte";

//计时器
double cost_time_;
clock_t start_time_;
clock_t end_time_;

int main()
{
    
    //--------------------- 1. Set up training data ---------------------------------------
    Mat trainData;
    Mat labels;
    trainData = read_mnist_image(trainImage);
    labels = read_mnist_label(trainLabel);

    cout << trainData.rows << " " << trainData.cols << endl;
    cout << labels.rows << " " << labels.cols << endl;

    //------------------------ 2. Set up the support vector machines parameters --------------------
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    //svm->setDegree(10.0);
    svm->setGamma(0.01);
    //svm->setCoef0(1.0);
    svm->setC(10.0);
    //svm->setNu(0.5);
    //svm->setP(0.1);
    svm->setTermCriteria(TermCriteria(CV_TERMCRIT_EPS, 1000, FLT_EPSILON));



    //------------------------ 3. Train the svm ----------------------------------------------------
    cout << "Starting training process" << endl;
    start_time_ = clock();
    svm->train(trainData, ROW_SAMPLE, labels);
    end_time_ = clock();
    cost_time_ = (end_time_ - start_time_) / CLOCKS_PER_SEC;
    cout << "Finished training process...cost " << cost_time_ << " seconds..." << endl;
    
    //------------------------ 4. save the svm ----------------------------------------------------
    svm->save("mnist_dataset/mnist_svm.xml");
    cout << "save as /mnist_dataset/mnist_svm.xml" << endl;

    
    //------------------------ 5. load the svm ----------------------------------------------------
    cout << "开始导入SVM文件...\n";
    Ptr<SVM> svm1 = StatModel::load<SVM>("mnist_dataset/mnist_svm.xml");
    cout << "成功导入SVM文件...\n";


    //------------------------ 6. read the test dataset -------------------------------------------
    cout << "开始导入测试数据...\n";
    Mat testData;
    Mat tLabel;
    testData = read_mnist_image(testImage);
    tLabel = read_mnist_label(testLabel);
    cout << "成功导入测试数据！！！\n";

    
    float count = 0;
    for (int i = 0; i < testData.rows; i++) {
        Mat sample = testData.row(i);
        // imshow("first image", sample);
        // waitKey(0);
        float res = svm1->predict(sample);
        res = std::abs(res - tLabel.at<unsigned int>(i, 0)) <= 0.05 ? 1.f : 0.f;
        count += res;
    }
    cout << "正确的识别个数 count = " << count << endl;
    cout << "错误率为..." << (10000 - count + 0.0) / 10000 * 100.0 << "%....\n";
    
    // system("pause");
    return 0;
}

// main.cpp