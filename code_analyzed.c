//********************************************************************************
//            Author:          tanhehe
//            DateTime:	       Mon Aug 26 19:34:47 2013
//            SearchMe:        http://www.cnblogs.com/tanhehe
//	      email:	       443016215@qq.com
//	      程序名:           CodeAnalyze
//            Description:     代码分析助手	
//
//********************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>

#define OVECCOUNT 30
#define EBUFLEN 128
#define BUFLEN 10024

typedef enum regex_flag {REG_FUNCTION, REG_MACRO, REG_HEADERS, REG_VARIABLES}regex_flag;

union regex_content {
    char func[100][5][100];	/* 存储函数信息 */
    char macr[100][3][30];	/* 存储宏信息 */
    char head[100][2][30];	/* 存储头文件信息 */
};

typedef struct regex_buf {
    int cn;
    regex_flag flag;
    union regex_content buf;
}regex_buf;

void read_file(char *filename, char *buf);
int regex_f(char *src, char *pattern, regex_buf *re_buf, regex_flag flag);
    
char buf[BUFLEN];

int main(int argc, char *argv[]) {
    char	*ifilename		= "main.c";
    char	*ofilename		= "out.txt";
    char	*pattern_function	= "((\\w+\\s+\\*?)(?#返回值)\\s*+(\\w+)(?#函数名)\\s*\\(((.(?![\"=><])(?#参数内不能包含引号))*)(?#参数)\\))\\s*{";
    char	*pattern_macro		= "#\\s?define\\s+(\\w+)\\s+(.+)";
    //char	*pattern_variables	= "(\\w+)\\s+(\\w+);";
    char	*pattern_headers	= "#\\s*include\\s*<(.+)>";

    FILE	*fp;
    regex_buf	re_buf;
    int		i;

    if(argc < 2) {
	fprintf(stderr, "缺少源文件\n");
	return 1;
    }
    if(argc >= 2) {
	ifilename = argv[1];
    }
    if(argc >= 3) {
	ofilename = argv[2];
    }

    if((fp = fopen(ofilename, "w")) == NULL) {
	fprintf(stderr, "open file error!\n");
    }
        
    read_file(ifilename, buf);
    printf("成功读取%s......\n", ifilename);
    
    fprintf(fp, "\n##########################头文件########################\n\n");
    if(regex_f(buf, pattern_headers, &re_buf, REG_HEADERS) == 0) {
	for(i=0; i<re_buf.cn; i++) {
	    fprintf(fp, "%d.头文件: %20s\n", i+1, re_buf.buf.head[i][1]);
	}
    }
    printf("已分析完毕头文件......\n");
    
    fprintf(fp, "\n##########################宏##########################\n\n");
    if(regex_f(buf, pattern_macro, &re_buf, REG_MACRO) == 0) {
	for(i=0; i<re_buf.cn; i++) {
	    fprintf(fp, "%d.宏原型: %s\n", i+1, re_buf.buf.macr[i][0]);
	    fprintf(fp, "宏名: %20s\n值: %20s\n", re_buf.buf.macr[i][1], re_buf.buf.macr[i][2]);
	}
    }
    printf("已分析完毕宏定义......\n");

    fprintf(fp, "\n########################函数#########################\n\n");
        
    if(regex_f(buf, pattern_function, &re_buf, REG_FUNCTION) == 0) {
	for(i=0; i<re_buf.cn; i++) {
	    fprintf(fp, "%d.函数原型: %s\n", i+1, re_buf.buf.func[i][1]);
	    fprintf(fp, "函数返回值: %10s\n函数名: %15s\n参数: %20s\n\n", re_buf.buf.func[i][2], re_buf.buf.func[i][3], re_buf.buf.func[i][4]);
	}
    }
    
    printf("已分析完毕函数......\n");

    fclose(fp);
    printf("已成功写入%s......\n", ofilename);
    return 0;
}

void read_file(char *filename, char *buf) {
    FILE	        *fp;
    char		temp[1000];

    if((fp = fopen(filename, "r")) == NULL) {
	fprintf(stderr, "readerr!\n");
	exit(1);
    }
  
    buf[0] = '\0';
  
    while(fgets(temp, sizeof(temp)-1, fp)) {
	strcat(buf, temp);
    }
}


int regex_f(char *src, char *pattern, regex_buf *re_buf, regex_flag flag) {
    pcre		*re;
    const char		*error;
    int			erroffset;
    int			ovector[OVECCOUNT];
    int			rc, i;
    int			cn = 0;

    re_buf->flag = flag; re_buf->cn = 0;
    
//    printf("String : %s\n", src);
//    printf("Pattern : \"%s\"\n", pattern);

    re = pcre_compile(pattern, 0, &error, &erroffset, NULL);
    if(re == NULL) {
	printf("PCRE compilation failed at offset %d: %s\n", erroffset, error);
	return 1;
    }

    rc = pcre_exec(re, NULL, src, strlen(src), 0, 0, ovector, OVECCOUNT);
    if(rc < 0) {
	if(rc == PCRE_ERROR_NOMATCH) printf("Sorry, no match ...\n");
	else printf("Matching error %d\n", rc);
	free(re);
	return -1;
    }

    for(i=0; i<rc; i++) {
	char *substring_start = src + ovector[2*i];
	int substring_length = ovector[2*i+1]-ovector[2*i];
	
	//printf("%2d: %.*s\n", i, substring_length, substring_start);
	switch(flag) {
	case REG_MACRO:
	    sprintf(re_buf->buf.macr[cn][i], "%.*s", substring_length, substring_start);
	    break;
	case REG_FUNCTION:
	    sprintf(re_buf->buf.func[cn][i], "%.*s", substring_length, substring_start);
	    break;
	case REG_HEADERS:
	    sprintf(re_buf->buf.head[cn][i], "%.*s", substring_length, substring_start);
	    break;
	    //  扩展
	}
    }
    cn++;
    
    for(;;) {
	int start_offset = ovector[1];
	rc = pcre_exec(re,  NULL, src, strlen(src), start_offset, 0, ovector, OVECCOUNT);

	if(rc == PCRE_ERROR_NOMATCH) { /* 匹配完毕 */
	    break;
	}
	else if(rc < 0) {	/* 匹配错误 */
	    //printf("Matching error %d\n", rc);
	    pcre_free(re);
	    return 1;
	}
	else if(rc == 0) {			/* 匹配, 但空间不足 */
	    rc = OVECCOUNT/3;
	    printf("ovector only has room for %d captured substring\n", rc-1);
	    return 1;
	}

	for(i=0; i<rc; i++) {
	    char *substring_start = src + ovector[2*i];
	    int substring_length = ovector[2*i+1] - ovector[2*i];
//	    printf("%2d: %.*s\n", i, substring_length, substring_start);
	    switch(flag) {
	    case REG_MACRO:
		sprintf(re_buf->buf.macr[cn][i], "%.*s", substring_length, substring_start);
		break;
	    case REG_FUNCTION:
		sprintf(re_buf->buf.func[cn][i], "%.*s", substring_length, substring_start);
		break;
	    case REG_HEADERS:
		sprintf(re_buf->buf.head[cn][i], "%.*s", substring_length, substring_start);
		break;
		// 扩展
	    }
	}
	cn++;
    }

    pcre_free(re);

    re_buf->cn = cn;
    return 0;
}

