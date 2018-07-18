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
[我的博客](http://blog.csdn.net/guodongxiaren)

## 环境

+ Ubuntu
+ C++
    + CImg
+ Python
    + Keras

## 结果
