# CalculateAngleAndHeightOfImages
Calculating the object's angle and height in the image

编程语言：C++

需要的库：opencv 3.4.1

-------------------------------------

计算一幅图像中某一物体的夹角和高度，如图所示为原图：

![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/CalculateAngleAndHeightOfImage/images/IMG_3289.JPG?raw=true)

需要计算的夹角和高度如下所示：

![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/CalculationSchematic.png?raw=true)



使用opencv计算的一种方法：

- **高度**：图像分割—边缘检测—关键点检测—kmeans得到最终点—计算距离

![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/height/%E5%8E%9F%E5%9B%BE.PNG?raw=true)

![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/height/%E5%88%86%E5%89%B2%E5%9B%BE.PNG?raw=true)

![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/height/%E9%AB%98%E5%BA%A6.PNG?raw=true)

- **夹角**：原图—图像分割—边缘检测—直线检测—计算角度

  ![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/angle/%E5%8E%9F%E5%9B%BE.PNG?raw=true)

  ![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/angle/%E9%98%88%E5%80%BC.PNG?raw=true)

  ![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/angle/%E8%BE%B9%E7%BC%98.PNG?raw=true)

  ![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/angle/%E7%9B%B4%E7%BA%BF.PNG?raw=true)

  -----------------------------------------

  最终结果：

  ![](https://github.com/songh1024/CalculateAngleAndHeightOfImages/blob/master/results/result.PNG?raw=true)

  

  

  

  