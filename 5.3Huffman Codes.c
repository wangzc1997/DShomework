In 1953, David A. Huffman published his paper "A Method for the Construction of Minimum-Redundancy Codes", and hence printed his name in the history of computer science. 
As a professor who gives the final exam problem on Huffman codes, I am encountering a big problem: the Huffman codes are NOT unique. 
For example, given a string "aaaxuaxz", we can observe that the frequencies of the characters 'a', 'x', 'u' and 'z' are 4, 2, 1 and 1, respectively. We may either encode the symbols as {'a'=0, 'x'=10, 'u'=110, 'z'=111}, or in another way as {'a'=1, 'x'=01, 'u'=001, 'z'=000}, both compress the string into 14 bits. 
Another set of code can be given as {'a'=0, 'x'=11, 'u'=100, 'z'=101}, but {'a'=0, 'x'=01, 'u'=011, 'z'=001} is NOT correct since "aaaxuaxz" and "aazuaxax" can both be decoded from the code 00001011001001. 
The students are submitting all kinds of codes, and I need a computer program to help me determine which ones are correct and which ones are not.

Input Specification:
Each input file contains one test case. For each case, the first line gives an integer N (2≤N≤63), then followed by a line that contains all the N distinct characters and their frequencies in the following format:
c[1] f[1] c[2] f[2] ... c[N] f[N]
where c[i] is a character chosen from {'0' - '9', 'a' - 'z', 'A' - 'Z', '_'}, and f[i] is the frequency of c[i] and is an integer no more than 1000. The next line gives a positive integer M (≤1000), then followed by M student submissions. 
Each student submission consists of N lines, each in the format:
c[i] code[i]
where c[i] is the i-th character and code[i] is an non-empty string of no more than 63 '0's and '1's.

Output Specification:
For each test case, print in each line either "Yes" if the student's submission is correct, or "No" if not.

Note: The optimal solution is not necessarily generated by Huffman algorithm. 
Any prefix code with code length being optimal is considered correct.

Sample Input:
7
A 1 B 1 C 1 D 3 E 3 F 6 G 6
4
A 00000
B 00001
C 0001
D 001
E 01
F 10
G 11
A 01010
B 01011
C 0100
D 011
E 10
F 11
G 00
A 000
B 001
C 010
D 011
E 100
F 101
G 110
A 00000
B 00001
C 0001
D 001
E 00
F 10
G 11
Sample Output:
Yes
Yes
No
No

code:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

void PercDown(int *heap, int parent);//下滤：将H中以H->Data[p]为根的子堆调整为最小堆
void BuildMinHeap(int *heap);//已满足结构性，调整元素位置使满足堆序性  
int DeleteMin(int *heap);//删除并返回堆顶元素，同时调整结构维护堆序性 
void InsertMinHeap(int *heap, int weight);//堆中插入一个元素  
int CalWPL(int *freq);//Weight Path Length 最优编码权重路径长度
int IsPrefix(char *s1, char *s2);  //判断两个字符串是否属于前缀码关系  
int HasPrefixCode(char s[][200], int n);//判断n个字符串是否含有前缀码  

int main(){
	int N;
	scanf("%d", &N);
	int freq[256] = {0};
	/*{'0' - '9', 'a' - 'z', 'A' - 'Z', '_'},均包含在扩展ASCII中，扩展ASCII共256个字符 ，
	freq[256]各个位置全部初始化为零*/
	int i;
	for (i = 0; i < N; ++i) {
		char ch;
		int num;
		getchar();//处理 回车 以及 每组数据间的空格 
		scanf("%c %d", &ch, &num);
		freq[ch] = num;
	}
	int WPL = CalWPL(freq); //模拟构建huffman树过程，计算WPL
	int M;//M个测试数据
	scanf("%d", &M);
	while (M--) {
		char ch[256];//存输入字符 
		char s[256][200];//存对应字符的哈夫曼编码 
		int thisWPL = 0;//该编码的WPL值 
		int i;
		for (i = 0; i < N; ++i) {
			scanf("\n%c %s", &ch[i], s[i]);
			thisWPL += freq[ch[i]] * strlen(s[i]);      //根据编码累加加权路径长  
		}
		if (thisWPL == WPL && !HasPrefixCode(s, N))     //同时满足加权路径长最短和不含前缀码，则满足huffman编码  
			printf("Yes\n");
		else
			printf("No\n");
	}
	return 0;
}

int CalWPL(int *freq){
	int heap[100] = { 0 };
	//huffman树所用堆，{'0' - '9', 'a' - 'z', 'A' - 'Z', '_'}范围不超过100个字符 
	int size = 0;
	int i;
	for (i = 0; i < 256; ++i) {   //将所有权值放入空堆中，等待建堆（调整使有堆序性）  
		if (freq[i]) {		//freq[]中不为零的位置，放入heap中。 
			size++;
			heap[size] = freq[i];
		}
	}
	heap[0] = size; //0位置保存堆中元素个数  
	BuildMinHeap(heap); //建立最小堆
	/*huffman树：每次从堆中弹出权值最小的两个子结点合并，合并后的父节点（权为子结点和）再入堆,
	WPL值 = 路径长度*权重 然后累加，也就是 非叶节点权重和（非叶节点的权重=连个子节点权重的和） */
	int CodeLen = 0;
	int j;
	for (j = 1; j < size; j++) {
		int weight1 = DeleteMin(heap);
		int weight2 = DeleteMin(heap);
		CodeLen += weight1 + weight2;
		InsertMinHeap(heap, weight1 + weight2);
	}
	return CodeLen;
}
void BuildMinHeap(int *heap){
	int i = heap[0] / 2;
	for (; i > 0; --i) //所有非叶节点根据堆序性下滤  
		PercDown(heap, i);
}
void PercDown(int *heap, int parent){
	int X = heap[parent];
	int child;
	for (; parent * 2 <= heap[0]; parent = child){
		child = 2 * parent;
		if (child + 1 <= heap[0] && heap[child + 1] < heap[child])
			child++;//Child指向左右子结点的较小者
		if (heap[child] > X) break;
		else heap[parent] = heap[child];
	}
	heap[parent] = X;
}
int DeleteMin(int *heap) {
	int minElem = heap[1];
	heap[1] = heap[heap[0]--];          //将最后一个元素移到堆顶  
	PercDown(heap, 1);             //唯一不满足堆序性的堆顶元素下滤  
	return minElem;
}
void InsertMinHeap(int *heap, int weight){
	heap[++heap[0]] = weight;           //插入到堆末尾  
	int i = heap[0] / 2;
	for (; i > 0 && heap[i] > weight; i /= 2)
		PercDown(heap, i);
}
int HasPrefixCode(char s[][200], int n){
	int i;
	for (i = 0; i < n; ++i){
		int j;
		for (j = i + 1; j < n; ++j){
			if (IsPrefix(s[i], s[j]))   //s[][]中所有编码形式 两两做对比  
				return 1;
		}
	}
	return 0;
}
int IsPrefix(char *s1, char *s2){
	while (s1 && s2 && *s1 == *s2)      //循环至第一个不相同的字母或结束位置  
		++s1, ++s2;
	if (*s1 == '\0' || *s2 == '\0')     //如果此时有一个到达结束位置，说明这个字符串一定是另一个字符串的前缀  
		return 1;
	else return 0;
}
