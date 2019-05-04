#ifndef DBSN_H
#define DBSN_H

#define     REC_DELETED      1
#define     OUT_OF_RANGE     2
#define     DBN_IS_EMPTY     4
#define     REC_EMPTY        8
#define     BAD_SUBSTNUMBER  16

#define     FATAL_ERROR      256
#define     BAD_PTR          256
#define     BAD_NUMBER       512
#define     DBS_NOT_EXIST    1024
#define     DBN_NOT_EXIST    2048
#define     BUFFER_SMALL     4096
#define     BAD_TABLE_NAME   8192
#define     BAD_TABLE_HANDLE 16384
#define     DBR_NOT_EXIST    32768
#define     NOT_A_BRACKET    65536

#define     ALL_FRAGM 0
#define     ACTIVE_FRAGM 1
#define     ALL_BACK 2
#define     ACTIVE_BACK 3

#define     BEG_BRACKET      0x80000000
#define     END_BRACKET      0x00000000

#include <sys/types.h> //chsise -> ftruncate
#include <unistd.h>  // close
#include <string.h>
#include <ctype.h>   // toupper
#include <stdio.h>

//---------------------------------------------------------------------------

//-- ������� ������ ������ ����� �����
extern "C" int        
	getLongArray( int count );

//-- �������� 2 ������ � ������ �������� � ������������� ��������
extern "C" long         
	compSpec( char* pc, char* pc1);

//-- �������� ������� ��������� 2� ����� � �������������� ��������
extern "C" long     
	compSpec0( char* pc, char* pc1);

//-- ��������� ����� ������
extern "C" int     
	getLen( char* pc);

//-- ��������� ����� �������� � ������
extern "C" int     
	substCount( char* pc);

//-- �������� �������� ��������� � �������� �������
extern "C" int     
	getSubstOffset(char* pc, int nom_sub);

//-- ��������� ��������� �� ��������� �������
extern "C" int     
	checkSubst(  char* pc, char* templ, int nom_sub );

//-- �������� ��������� � �������� ������� �� ������
extern "C" int     
	getSubst(char* pc, char* pcbuf, int buf_size, int nom_sub);

//-- �������� ����� ����� �� ��������� � �������� �������
extern "C" int     
	getSubint(char* pc, int& ires, int nom_sub);

//-- ������������ ������ � ������ �� ������ �����
extern "C" int     
	formStint( char* pcbuf, int buf_size, int ival );

//== �������� ��������� � ������ �����
extern "C" int     
    addSubst(char* pc, char* pcres, int res_size, char* pcsubst);

//== �������� �������� ��������� � ������ �����
extern "C" int     
    addSubint(char* pc, char* pcres, int res_size, int ival);

//== �������� ��������� � ������ ����� ������
extern "C" int     
    insSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst);

//== �������� �������� ��������� � ������ ����� ������
extern "C" int     
    insSubint(char* pc, char* pcres, int res_size, int nom_sub,  int ival);

//== �������� ����� ���������
extern "C" int     
    substLen(char* pc, int nom_sub );

//-- �������� ��������� �� ����� �������� � ������ ����� ������
extern "C" int     
	setSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst);

//-- �������� �������� ��������� �� ����� �������� � ������ ����� ������
extern "C" int     
	setSubint(char* pc, char* pcres, int res_size, int nom_sub,  int ival );

//-- �������� ��������� (�������� �� ������ �������� )
extern "C" int     
    clearSubst(char* pc, int nom_sub );

//-- ��������� ������� ���������
extern "C" int     
    cutSubst(char* pc, int nom_sub );

//-- ��������� ������� ������ ��������
extern "C" int     
    cutSubstGroup(char* pc, int nom_sub, int cut_count );

//-- ������� ��� ��������� ����� �������
extern "C" int     
    cutLeft(char* pc, int nom_sub );

//-- ������� ��� ��������� ������� � �������
extern "C" int     
    cutRight(char* pc, int nom_sub );

//-- ������� ��� ������ ��������� ������
extern "C" int     
    cutRightEmpty(char* pc );

//-- ������� ��� ������  � ���������� ��������� ������
extern "C" int     
    cutRightWhile(char* pc );

//-- ���������� ��������� � ����������
extern "C" int     
    uniteSubst(char* pc, int nom_sub );

//-- ��������� ������� ��� 2� ��������
extern "C" int     
    compSubst(  char* pc1, char* pc2, int subst_nom, char* condition );

//==  �������� ��������� ������ ������( ���������)
extern "C" int       getTextHead(char* pc, char* pcbuf, int bufsize);

//==  �������� ������� ������ ��� ��������� ������( ��������� )
extern "C" int       getTextBody(char* pc, char* pcbuf, int bufsize);

//-- ������ ������ ����� � �������� ����
extern "C" int         
	fwriteInt( int fh, int val);

//-- ������ ������ ����� �� ��������� �����
extern "C" int         
	freadInt( int fh );

//== ��������� ����� ����� � ������ =
extern "C" int         
	countTextSt(char* pc);

//== �������� ������ � �������� ������� �� ������
extern "C" int       
	getTextSt(char* pc, char* pcbuf, int bufsize,  int st_nom);

//== ������� �������� �������� ��������� ���� DBSN 
extern "C" int         
	getFragmOffset( int  dbhadr,  int &rec_offset );

//== ��������� �����  �������� ��������� ���� DBSN
extern "C" int     
	getFragmSize( int  dbhadr, int &rec_size );

//== ��������� ������������ ����� ��������� ���� DBSN
extern "C" int         
	getMaxFragmSize( int  dbhadr );

//== ��������� ������� �� ������
extern "C" int         
	isActive( int dbhadr );

//==  ��������  ������� ������ DBSN
extern "C" int         
	getErrCod( int dbhadr );

//== �������� ������� ������
extern "C" int         
	clearError( int dbhadr );

//-- ����� ������� ----
extern "C" char*       
	trim( char* pc, char* pcres);

//== ������� ������ ���� ���������� �� ����� �����
extern "C" int          
	createDBSN( char*  fil_name );

//== ��������� ������������� ������ DBSN
extern "C" int          
	existDBSN( char*  fil_name );

//== �������� ��� ���������� DBS �����
extern "C" int       
	getAllDBS( char* fil_name, char* &text );

//== ������� ���� ���������� DBSN
extern "C" int       
	openDBSNA( char* fil_name );

//== ������� ���� ���������� DBSN � ������ �� ������ �������� �����
extern "C" int    
	openDBSN( char* fil_name );

//== ������� ���� ���������� DBSN
extern "C" int       
	closeDBSN( int &dbhadr );

//==  ��������� ����� ���� ������� � ���� DBSN
extern "C" int     
	countFragmA( int dbhadr );

//==  ��������� ����� ������� (�������� ) � ���� DBSN
extern "C" int     
	countFragm( int dbhadr );

//== ������� ��� ��������� ���� DBSN
extern "C" int     
	delAllFragm(  int dbhadr );

//== ��������� ����������  ���� DBSN
extern "C" int       
	eraseDBSN( char*  fil_name);

//==  ������������� ���� DBSN � ����� ���
extern "C" int        
	renameDBSN( char*  old_fil, char*  new_fil );

//== ��������� ���� DBSN
extern "C" int     
	packDBSN( char*  fil_name );

//== ������� ������� �� ���� DBSN  �� �������� �������
extern "C" int     
    selectDBSN(  int dbhadr, int fhr, char* new_tabl );

//== ��������� �� ����� ����� ���� DBSN
extern "C" int        
	isEndDBSN( int dbhadr );

//==  �������� ����� �������� ���������
extern "C" int        
	getNom(int dbhadr );

//==  ���������� ����� �������� ���������
extern "C" int      	 
	setNom(int dbhadr, int new_nom );

//==  ��������� ����� �������� ���������  
extern "C" int        
	incNom(int dbhadr, int  count );

//==  ��������� ����� �������� ���������
extern "C" int        
	decNom(int dbhadr, int  count );

//==  ���������� ������ ����� ���������
extern "C" int        
	setFirst(int dbhadr );

//==  ���������� ��������� ����� ���������
extern "C" int        
	setLast(int dbhadr );

//== ������� ������� �������� ���� DBSN  
extern "C" int     
	getFragm(  int dbhadr, char* fragm,  int bufsize );

//== ��������� ������ �������� ���� DBSN
extern "C" int     
	getFirstFragm(  int dbhadr, char* fragm,  int bufsize );

//== ��������� ��������� �������� ���� DBSN
extern "C" int     
	getNextFragm(  int dbhadr, char* fragm,  int bufsize );

//== ��������� ����������� �������� ���� DBSN
extern "C" int     
	getPrevFragm(  int dbhadr, char* fragm,  int bufsize );

//== ��������� ������ �������� ���� DBSN
extern "C" int     
	getLastFragm(  int dbhadr, char* fragm,  int bufsize );

//== ��������� �������� �������� ���� DBSN
extern "C" int     
	readFragm( int* dbh, char* fragm, int bufsize, int fragm_nom );

//==  �������� ������ � ���� ���������� DBSN
extern "C" int     
    addFragm( int dbhadr, char* fragm );

//==  �������� ����� � ������� ������ ���� DBSN
extern "C" int     
	setFragm(  int dbhadr, char*  fragm );

//==  �������� ����� � �������� ������ ���� DBSN
extern "C" int     
	writeFragm(int dbhadr, char* fragm, int fragm_nom );

//== ������� �������� ���� DBSN � �������� �������
extern "C" int     
	delFragm(  int dbhadr );

//== ����� ������� �������� � ��������� ���� DBSN
extern "C" int        
	undelFragm(  int dbhadr );

//== ��������� ����� ������ ���������� ���� DBSN
extern "C" int        
	countNblocks( int dbhadr, int block_size, int flag_sel );

//== ������� �������� ���� ������� ���������� ���� DBSN
extern "C" int        
	getNblock( int dbhadr, int* pblock, int block_nom,  int block_size, int flag_sel  );

//-- ��������� ����� �������� � �������� ������
extern "C" int      
	binsubstCount( char* pcFragm, int fragmLen);

//-- �������� �������� �������� ���������
extern "C" int     
	binsubstOffset(char* pcFragm, int fragmLen, long nom_sub);

//-- ��������� ����� ��������� � ������ �� �� ������
extern "C" int     
	binsubstLen( char* pcFragm, int fragmLen, long nom_sub );

//-- ��������� ������ ��������� � ������ �� �� ������
extern "C" int     
	binsubstSize( char* pcFragm, int fragmLen, long nom_sub );

//-- �������� �������� ��������� � �������� ������� �� ������ � �����
extern "C" int     
	getBinsubst(char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub);

//== �������� � ��������� �������� ���������
extern "C" int     
	addBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen,  char* pcBinsubst );

//== �������� �������� ��������� �� ���������
extern "C" int     
	cutBinsubst( char* pcFragm, int fragmLen, long nom_sub );

//== �������� �������� ���������  � ���������
extern "C" int     
    clearBinsubstIn( char* pcFragm, int fragmLen, long nom_sub );

//== �������� �������� ��������� ����� �������� �� ��������
extern "C" int     
	insBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst );

extern "C" int     
	setBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinSubst);

//==  �������� ��� ������ � ���� ���������� DBSN
extern "C" int          
	addBinFragm( int dbhadr, char* fragm, int len_fragm );

//==  �������� ��� �������� � ������� ������ ���� DBSN
extern "C" int        
	setBinFragm(  int dbhadr, char*  fragm, int len_fragm );

//==  ������� � ������� ��������� ���������
extern "C" int     
    gotoFirst( int dbhadr );

//==  ������� ������ ����� ��������� �������� ����������
extern "C" int     
    goForward( int dbhadr, int count );

//==  ������� ����� ����� ��������� �������� ����������
extern "C" int     
    goBack( int dbhadr, int count );

//==  ������� � ���������� ��������� ���������
extern "C" int     
    gotoLast( int dbhadr );

//==  ������� ������ � ���������� ��������� ���������
extern "C" int     
    gotoNext( int dbhadr );

//==  ������� ����� � ����������� ��������� ���������
extern "C" int     
    gotoPrev( int dbhadr );

extern "C" int*     
	writeBinRec( int dbhadr, int* plbuf);

extern "C" int*      
	readBinRec( int dbhadr, int* plbuf);

extern "C" int      
	xchgFragm( int  dbhadr, int  dest_nom );

extern "C" int      
	flushDBSN( int  dbhadr );

//== ��������� �������� ������ ���������  �� dbn
extern "C" int     
	cutFragm(  int dbhadr );

//==  �������� ������ � ���� ���������� DBSN � ������� �������
extern "C" int     
	insFragm( int dbhadr, char* fragm);

//== ����������� ������� ��������� � �������� ����� dbn
extern "C" int     
	moveFragm(  int dbhadr, int  dest_nom );

//== �������� ������� ������� � ������� ���������
extern "C" int     
	xchgFragm( int  dbhadr, int  dest_nom );

//== ����������� ���� ���������� ��� ������ ������
extern "C" int     
	copyDBSN( char*  fil_name, char*  copy_filname, int flag_pack  );

//== ���������� � ����� �������� ������ � ������������
extern "C" int     
	getErrMes( int dbhadr, char* err_buf, int buf_len, char* comment);

//== ������ �� ��������

//== ������� ��������� ���� �� ������� ���������� �� ����� �����
extern "C" int    
	createDBRF( char*  fil_name);

//== ������� ��������� ���� �� ������� ���������� DBSN
extern "C" int    
	openDBRF( char* filref_name );

//== ������������ ��������� ���� �� DBSN � �������
extern "C" int    
	formDBRF(  int dbhadr, int fhr, int subst_nom, char* condition, char* value );

//== ������� ��������� ���� �� ������� ���������� DBSN
extern "C" int   
	closeDBRF( int fhr );

//== ������������ ��������� ���� � ����� � ��������� ���������
extern "C" int   
	copyDBRF( char*  fil_ref, char*  copy_ref, int flag_pack );

//== ��������� ���� ������
extern "C" int    
    packDBRF(  int fhr );

//== ���������� ���������� ������� � ����� ������
extern "C" int    
	countRef( int fhr );

//== �������� ����� ��������� �� ������ �� ����
extern "C" int     
	getRef(  int fhr, int ref_nom );

//== �������� �������� �� ������ �� ����
extern "C" int     
	getFragmByRef(  int dbhadr, int fhr, int ref_nom,  char* fragm, int bufsize );

//== �������� � ��������� ���� ����� ���������
extern "C" int     
	addRef(  int fhr, int fragm_nom );

//== �������� � ��������� ����� ����� ��������� � ������ �������
extern "C" int     
	setRef(  int fhr, int ref_nom, int fragm_nom );

//== ������� � ��������� ����� ����� ��������� � ������ �������
extern "C" int     
	delRef(  int fhr, int ref_nom );

//== ����� �������� � ��������� �����
extern "C" int     
	undelRef(  int fhr, int ref_nom );

//== �������� ������� ������ �� ���������
extern "C" int       
	xchgRef( int  fhr, int  ref_nom1,  int ref_nom2 );

//==  �������� ��������� � ��������� ����� ����� ��������� � ������ �������
extern "C" int     
	cutRef(  int fhr, int ref_nom );

//== �������� � ��������� ����� ����� ��������� � ������ �������
extern "C" int    
	insRef(  int fhr, int ref_nom, int fragm_nom );

//== �������� � ��������� ����� ����� ��������� ����� ������ �������
extern "C" int    
	insRefAfter(  int fhr, int ref_nom, int fragm_nom );
	
//== ����������� ������ � ����� ������� �� �������
extern "C" int    
	moveRef(  int fhr, int ref_nom1, int ref_nom2 );

//== ������� ���� ������ �� ��������� �������
extern "C" int    
	pushRblock(  int fhr, int ref_nom, int block_len );

//== ����� ������ �����( ����������� ������ )
extern "C" int    
	findRblock(  int fhr, int ref_nom );

//== ����� ��������������� ����������� ������
extern "C" int    
	endRblock(  int fhr, int ref_nom );

//== ����� ��������������� ����������� ������ (�����)
extern "C" int    
	begRblock(  int fhr, int ref_nom );

//== ������ ���� ������ �� ���������� �������
extern "C" int    
	popRblock(  int fhr, int ref_nom );

//== �������� ���� �� ����� ������
extern "C" int    
	cutRblock(  int fhr, int ref_nom, int block_len );

//== ����������� ���� ������ � ����� ������� �����
extern "C" int    
	moveRblock(  int fhr, int new_nom, int ref_nom, int block_len );

//== �������� ���� ������ �� ������� �����
extern "C" int    
	addRblock(  int fhr, int fhr2, int ref_nom2, int block_len );

//== �������� ���� ������ �� ������� �����
extern "C" int    
	insRblock(  int fhr, int ref_nom,int fhr2, int ref_nom2, int block_len );

//== �������� ������ �� ��������� �����
extern "C" int    
    clearFiltr(  int fhr  );

//== �������� ������ �� ��������� ����
extern "C" int    
    addFiltr(  int dbhadr, int fhr, int subst_nom, char* condition, char* value );

//== ������������� ��������� ���� �� �������
extern "C" int    
    sortDBRF(  int dbhadr, int fhr, int subst_nom, char* condition );


//== ���������� ����� ����� � �������� ����� ������ �������� ���������
extern "C" int    
    setBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long src );

//== �������� ����� � �������� ������� �� ������ �������� ���������
extern "C" int    
    getBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long &dst );

//== ������� ����� � �������� ������� �� ������ �������� ���������
extern "C" int    
    cutBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num );

//== �������� ����� ����� � ����� ������ �������� ���������
extern "C" int    
    addBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long src );

//== �������� ����� ����� � �������� ����� ������ �������� ���������
extern "C" int    
    insBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long nom_num, long src );

//== �������� ������� 2 ����� � ������ �������� ���������
extern "C" int    
    xchgBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num1, long nom_num2 );

//== ������� DBS ���� �� ��������� �� ��������� �����
extern "C" int    
	formDBSN( char* txtFile, char* keyWord, int add_key);

#endif