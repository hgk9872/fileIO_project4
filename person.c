#include <stdio.h>
#include <stdlib.h>
#include "person.h"
#include <string.h>
#include <unistd.h>
//필요한 경우 헤더 파일과 함수를 추가할 수 있음

// 과제 설명서대로 구현하는 방식은 각자 다를 수 있지만 약간의 제약을 둡니다.
// 레코드 파일이 페이지 단위로 저장 관리되기 때문에 사용자 프로그램에서 레코드 파일로부터 데이터를 읽고 쓸 때도
// 페이지 단위를 사용합니다. 따라서 아래의 두 함수가 필요합니다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장한다
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장한다
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터를 저장합니다. 그리고 난 후 writePage() 함수를 호출하여 수정된 pagebuf를
// 레코드 파일에 저장합니다. 반드시 페이지 단위로 읽거나 써야 합니다.
//
// 주의: 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근해서 처리한다.

//
// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//

int numPage; // 전체 데이터 페이지 수
int numRecord; // 모든 레코드 수
int iPage; // 가장 최근 삭제된 페이지 번호
int iRecord; // 가장 최근 삭제된 레코드 번호


void readPage(FILE *fp, char *pagebuf, int pagenum)
{

	fseek(fp, 16+PAGE_SIZE*pagenum, SEEK_SET); //주어진 페이지넘버로 이동
	fread((void *)pagebuf, PAGE_SIZE, 1, fp);


}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다. 
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{

	fseek(fp, 16+PAGE_SIZE*pagenum, SEEK_SET);
	fwrite((void *)pagebuf, PAGE_SIZE, 1, fp);

}

//
// 새로운 레코드를 저장할 때 터미널로부터 입력받은 정보를 Person 구조체에 먼저 저장하고, pack() 함수를 사용하여
// 레코드 파일에 저장할 레코드 형태를 recordbuf에 만든다. 
// 
void pack(char *recordbuf, const Person *p)
{
	strcpy(recordbuf, p->id);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->name);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->age);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->addr);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->phone);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->email);
	strcat(recordbuf, "#");
}

// 
// 아래의 unpack() 함수는 recordbuf에 저장되어 있는 레코드를 구조체로 변환할 때 사용한다.
//
void unpack(const char *recordbuf, Person *p)
{
	

}

//
// 새로운 레코드를 저장하는 기능을 수행하며, 터미널로부터 입력받은 필드값들을 구조체에 저장한 후 아래 함수를 호출한다.
//
void add(FILE *fp, const Person *p)
{
	char *pagebuf; // 페이지 버퍼
	char *recordbuf; // 레코드 버퍼
	int *headerbuf; // 페이지헤더 버퍼

	int numPageRecord; // 해당 페이지에 있는 레코드 수
	int offset;
	int recordLen;
	
	int numRecord;
	int numPage;
	int iPage;
	int iRecord;
	fseek(fp, 0, SEEK_SET);
	fread(&numPage,sizeof(int),1,fp);
	fread(&numRecord,sizeof(int),1,fp);
	fread(&iPage,sizeof(int),1,fp);
	fread(&iRecord,sizeof(int),1,fp);

	headerbuf = (int *)malloc(HEADER_AREA_SIZE); //25
	recordbuf = (char *)malloc(MAX_RECORD_SIZE);
	pagebuf = (char *)malloc(PAGE_SIZE);

	pack(recordbuf, p);


	//데이터가 아무것도 입력되지 않은 경우
	if (numRecord == 0){
		numPage++; // 처음 입력이므로 페이지 수 증가
		numPageRecord = 1; 
		offset = 0; 
		recordLen = strlen(recordbuf);

		//headbuf에 입력
		memcpy(headerbuf, &numPageRecord, 4);
		memcpy(headerbuf + 1, &offset, sizeof(int));
		memcpy(headerbuf + 2, &recordLen, sizeof(int));

		//pagebuf 생성
		memcpy(pagebuf, headerbuf, HEADER_AREA_SIZE);
		memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));

		writePage(fp, pagebuf, 0);
	}

	//기존 데이터가 하나라도 있는 경우
	else{
		if(iRecord != -1){
			int page;
			int deleteLen;
			readPage(fp, pagebuf, iPage); //해당 페이지 읽어옴
			memcpy(&deleteLen, pagebuf + 8 + 4 * iRecord, sizeof(int)); //해당 레코드번호에 해당하는 레코드 길이 추출
			memcpy(&offset, pagebuf + 4 + 4*iRecord, sizeof(int)); //해당 레코드번호에 해당하는 offset
			if (deleteLen >= strlen(recordbuf)){ //right size인 경우
				int page = iPage; // 현재 페이지 번호 저장
				memcpy(&iPage, pagebuf + HEADER_AREA_SIZE + offset + 1, sizeof(int)); //삭제된 레코드에 저장되어있는 페이지와 레코드번호 저장
				memcpy(&iRecord, pagebuf + HEADER_AREA_SIZE + offset + 5, sizeof(int));
				memcpy(pagebuf + HEADER_AREA_SIZE + offset, recordbuf, strlen(recordbuf)); //삭제된 공간에 데이터 덮어쓰기
				writePage(fp, pagebuf, page); //페이지 단위로 저장

				//헤더파일 부분 삭제된 페이지, 레코드 수정
				numRecord--;
				fseek(fp, 8, SEEK_SET); 
				fwrite(&iPage, 4, 1, fp);
				fwrite(&iRecord, 4, 1, fp);
			}
			else{ //right size가 아닌 경우 
				readPage(fp, pagebuf, numPage-1);
				memcpy(&numPageRecord, pagebuf, sizeof(int)); //제대로 출력됨.
				if(numPageRecord == 1){ //하나 저장되어 있는 경우
				
					numPageRecord++;
					recordLen = strlen(recordbuf) ;
					
					memcpy(&offset, pagebuf + 8, sizeof(int)); // offset 값 추출
					memcpy(pagebuf, &numPageRecord, sizeof(int));
					memcpy(pagebuf + 12, &offset, sizeof(int));
					memcpy(pagebuf + 16, &recordLen, sizeof(int));
					memcpy(pagebuf + HEADER_AREA_SIZE + offset, recordbuf, strlen(recordbuf)); //문제
					
					writePage(fp, pagebuf, numPage-1);
			}
				else if(numPageRecord == 2){ //두개 저장되어있는 경우 -> 새로운 페이지 할당
					numPage++;
					numPageRecord = 1; 
					offset = 0; 
					recordLen = strlen(recordbuf);

					//headbuf에 입력
					memcpy(headerbuf, &numPageRecord, sizeof(int));
					memcpy(headerbuf + 1, &offset, sizeof(int));
					memcpy(headerbuf + 2, &recordLen, sizeof(int));

					//pagebuf 생성
					memcpy(pagebuf, headerbuf, HEADER_AREA_SIZE);
					memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));

					writePage(fp, pagebuf, numPage-1);
					}
			}

		} //삭제된 데이터가 있는 경우.
		else{ //없는 경우
			readPage(fp, pagebuf, numPage-1);
			memcpy(&numPageRecord, pagebuf, sizeof(int)); //제대로 출력됨.
			if(numPageRecord == 1){ //하나 저장되어 있는 경우
				
				numPageRecord++;

				recordLen = strlen(recordbuf);
				memcpy(&offset, pagebuf + 8, sizeof(int)); // offset 값 추출
				memcpy(pagebuf, &numPageRecord, sizeof(int));
				memcpy(pagebuf + 12, &offset, sizeof(int));
				memcpy(pagebuf + 16, &recordLen, sizeof(int));
				memcpy(pagebuf + HEADER_AREA_SIZE + offset, recordbuf, strlen(recordbuf));

				writePage(fp, pagebuf, numPage-1);
			}
			else if(numPageRecord == 2){ //두개 저장되어있는 경우 -> 새로운 페이지 할당
				numPage++;
				numPageRecord = 1; 
				offset = 0; 
				recordLen = strlen(recordbuf);

				//headbuf에 입력
				memcpy(headerbuf, &numPageRecord, sizeof(int));
				memcpy(headerbuf + 1, &offset, sizeof(int));
				memcpy(headerbuf + 2, &recordLen, sizeof(int));

				//pagebuf 생성
				memcpy(pagebuf, headerbuf, HEADER_AREA_SIZE);
				memcpy(pagebuf + HEADER_AREA_SIZE, recordbuf, strlen(recordbuf));

				writePage(fp, pagebuf, numPage-1);
				}
		}
	}

	numRecord++; //레코드 개수 증가
	fseek(fp, 0, SEEK_SET);
	fwrite(&numPage, 4, 1, fp);
	fwrite(&numRecord, 4, 1, fp);

	free(recordbuf);
	free(pagebuf);
	free(headerbuf);

}

//
// 주민번호와 일치하는 레코드를 찾아서 삭제하는 기능을 수행한다.
//
void delete(FILE *fp, const char *id)
{
	int numPageRecord;
	char *pagebuf;
	int offset;
	char *recordbuf;

	int numRecord;
	int numPage;
	int iPage;
	int iRecord;
	fseek(fp, 0, SEEK_SET);
	fread(&numPage,sizeof(int),1,fp);
	fread(&numRecord,sizeof(int),1,fp);
	fread(&iPage,sizeof(int),1,fp);
	fread(&iRecord,sizeof(int),1,fp);

	pagebuf = (char *)malloc(PAGE_SIZE);
	recordbuf = (char *)malloc(14);

	for(int i=0; i < numPage ; i++){ //페이지마다 탐색
		readPage(fp, pagebuf, i); //해당 페이지로 가서 페이지 가져옴.
		memcpy(&numPageRecord, pagebuf, sizeof(int)); // 해당 페이지 레코드 개수
		for(int j=0; j < numPageRecord ; j++){ //해당 페이지의 모든 레코드 검사
			memcpy(&offset, pagebuf + 4 + 8*j, sizeof(int));
			memcpy(recordbuf, pagebuf + HEADER_AREA_SIZE + offset, 14);
			char *ptr = strstr(recordbuf, id); //해당 레코드가 있는지 검사
			if(ptr != NULL){
				strncpy(ptr, "*", 1);
				memcpy(pagebuf + HEADER_AREA_SIZE + offset, ptr, 1);
				if(iPage == -1){ //삭제된 페이지가 존재하지 않는 경우
					memcpy(pagebuf + HEADER_AREA_SIZE + offset + 5, &iRecord, 4);
				}
				else{ //직전에 삭제된 페이지가 있을 때
					memcpy(pagebuf + HEADER_AREA_SIZE + offset + 5, &j, 4); // 레코드번호
				}
				memcpy(pagebuf + HEADER_AREA_SIZE + offset + 1, &iPage, 4); //삭제된 페이지 저장
				writePage(fp, pagebuf, i); //저장
				iPage = i;
				iRecord = j;
				fseek(fp, 8, SEEK_SET);
				fwrite(&iPage, 4, 1, fp);
				fwrite(&iRecord, 4, 1, fp);
			}
		}

	}
	free(pagebuf);
	free(recordbuf);
}

int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터
	
	Person p; //입력받을때 p에 저장

	if( access(argv[2], F_OK ) != -1 ) { //파일 존재하면
		fp = fopen(argv[2], "r+b");
		//printf("existed file open!\n");
		if(fp == NULL){
			printf("file open error\n");
			return -1;
		}
	} else { //존재하지 않는 경우 초기화
		fp = fopen(argv[2], "w+b");
		//printf("new file open!\n");
		if(fp == NULL){
			printf("file open error\n");
			return -1;
		}

		numPage = 0;
		numRecord = 0;
		iPage = -1;
		iRecord = -1;

		fwrite(&numPage, 4, 1, fp);
		fwrite(&numRecord, 4, 1, fp);
		fwrite(&iPage, 4, 1, fp);
		fwrite(&iRecord, 4, 1, fp);
	}
	
	//옵션 a add호출
	if(argv[1][0] == 'a'){
	strcpy(p.id, argv[3]);
	strcpy(p.name, argv[4]);
	strcpy(p.age, argv[5]);
	strcpy(p.addr, argv[6]);
	strcpy(p.phone, argv[7]);
	strcpy(p.email, argv[8]);

	add(fp, &p);
	}

	else if(argv[1][0] == 'd'){
		delete(fp, argv[3]);
	}
	

	fclose(fp);
		
	return 1;
}
