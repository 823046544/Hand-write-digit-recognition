# 手写数字识别


## 实现功能

  + A4纸矫正
    + Canny 图像边缘检测
    + Hough 变换检测直线
    + 角点检测
    + 图像映射实现矫正
  + 数字识别
    + Canny实现前后景分离
    + CNN实现数字识别

## 使用

1. 编译
```
cd src
make
```

2. 运行
```
./main [file_name]
```
对 `../Dataset/[file_name].jpg` 进行A4纸矫正以及数字识别

3. 运行测试用例
需要把程序中display注释掉~
```
python run_test.py
```

## 实现
[我的博客](https://blog.csdn.net/lzh823046544/article/details/81095974)

## 环境

+ Ubuntu
+ C++
    + CImg
+ Python
    + Keras

## 结果

* 读取图片

![这里写图片描述](https://github.com/823046544/Hand-write-digit-recognition/blob/master/Img/1.jpg)

* Canny 检测边缘

![这里写图片描述](https://github.com/823046544/Hand-write-digit-recognition/blob/master/Img/2.jpg)

* 检测四个角点

![这里写图片描述](https://github.com/823046544/Hand-write-digit-recognition/blob/master/Img/3.jpg)

* A4纸矫正

![这里写图片描述](https://github.com/823046544/Hand-write-digit-recognition/blob/master/Img/4.jpg)

* 数字分割

![这里写图片描述](https://github.com/823046544/Hand-write-digit-recognition/blob/master/Img/5.jpg)

* 最终结果：包含四个角点以及识别内容

![这里写图片描述](https://github.com/823046544/Hand-write-digit-recognition/blob/master/Img/0.jpg)