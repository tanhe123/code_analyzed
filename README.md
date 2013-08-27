# 软件名： code_analyze
帮助分析源码文件，包括文件中的头文件、宏定义、函数。	
## 用途：
无聊时，打发下时间。
## 演示：
对于本源程序的分析结果如下：

	##########################头文件########################
	
	1.头文件:              stdio.h
	2.头文件:             stdlib.h
	3.头文件:             string.h
	4.头文件:               pcre.h
	
	##########################宏##########################
	
	1.宏原型: #define OVECCOUNT 30
	宏名:            OVECCOUNT
	值:                   30
	2.宏原型: #define EBUFLEN 128
	宏名:              EBUFLEN
	值:                  128
	3.宏原型: #define BUFLEN 10024
	宏名:               BUFLEN
	值:                10024
	
	########################函数#########################
	
	1.函数原型: int main(int argc, char *argv[])
	函数返回值:       int 
	函数名:            main
	参数: int argc, char *argv[]
	
	2.函数原型: void read_file(char *filename, char *buf)
	函数返回值:      void 
	函数名:       read_file
	参数: char *filename, char *buf
	
	3.函数原型: int regex_f(char *src, char *pattern, regex_buf *re_buf, regex_flag flag)
	函数返回值:       int 
	函数名:         regex_f
	参数: char *src, char *pattern, regex_buf *re_buf, regex_flag flag
	

## 使用方法：
命令行使用方法是: 参数1:源程序文件,  参数2:输出文件名(可选项,默认out.txt)
## 截图


## 说明
因为在windows下，配置pcre库未成功，所以不打算弄个windows版本了。只有linux版本.



***源程序***： 

***源代码***：


## 详情请参见我的博客: [https://github.com/tanhe123](http://www.cnblogs.com/tanhehe/p/3279415.html)
