#include "dbsn.h"

//-- ������� ������ ������ ����� �����
int getLongArray( int count ) {
    int*  pl = new int[count];
    for( int j = 0; j<count; j++ )
        *(pl+j) = 0;
    return  (int) pl;
};

//-- ��������� ����� ������
int getLen( char* pc) {
    if( !pc ) return - BAD_PTR;
    int     len=0;
    while(*pc++)  len++;
    return  len;
}

//-- ��������� ����� �������� � ������
int substCount( char* pc) {
    if( !pc ) return - BAD_PTR;
    int     res = 1;
    for( ; *pc; pc++ )
        if( *pc =='|')  res++;
    return res;
};

//-- �������� ��������� � �������� ������� �� ������ � ����� pcres
int getSubst(char* pc, char* pcres, int res_size, int nom_sub) {
    if( !pc || !pcres ) return - BAD_PTR;
    int     len = getLen( pc),
            subst_count = substCount(pc);
    char    *pc_src = pc, *pc_dst = pcres;
    if(pc[len-1]=='\n')  pc[len-1]=0; //-- ������������� ��������� ����� ������
    *pcres = 0;
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    //-- ���� ����� ���� ���������
    if(( nom_sub == 0)&& (subst_count==1))
    {   do {
            if( pc_dst - pcres >= res_size - 1) {
                *pc_dst = 0;  return  - BUFFER_SMALL;
            };
            *pc_dst++ = *pc_src++;
        }while( *pc_src);
        *pc_dst = 0;      //-- 17 aug 2013
        return pc_dst - pcres - 1;
    }
    //-- �� ���� ��������� � ������� ��������� ���������
    if (nom_sub == subst_count-1)
    {   pc_src = pc + len;
        while(*pc_src != '|') pc_src--;
        for( pc_src++; *pc_src;){
            if( pc_dst - pcres >= res_size - 1) {
                *pc_dst = 0;  return  - BUFFER_SMALL;
            };
            *pc_dst++ = *pc_src++;
        }
        *pc_dst = 0;        //-- 12 mar 2013
        return pc_dst - pcres - 1;
    }
    //-- �� ��������� ���������
    for( ;nom_sub; pc_src++ )
    {   if( *pc_src != '|' ) continue;
        nom_sub--;
    }
    //-- ����� ������ ���������
    while((*pc_src)&&(*pc_src != '|')){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pcres - 1;
};

//==  �������� � ����� ��������� ������ ������( ���������)
//--  ���������� ����� ���������� ������
int getTextHead(char* pc, char* pcres, int res_size )
{
    if( !pc || !pcres ) return - BAD_PTR;
    char*  pc_dst = pcres;
    while((*pc)&&(*pc != '\n')&&(*pc != '\r')){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc++;
    }
    *pc_dst = 0;
    return  pc_dst - pcres - 1;
}

//==  �������� ������� ������ ��� ��������� ������( ��������� )
int getTextBody(char* pc, char* pcres, int res_size)
{
    if( !pc || !pcres ) return - BAD_PTR;
    while((*pc)&&(*pc != '\n'))
        pc++;
    if( *pc == '\n')
        pc++;
    char*  pc_dst = pcres;
    while(*pc){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc++;
    }
    *pc_dst = 0;
    return  pc_dst - pcres - 1;
}

//-- ������ ������ ����� � �������� ����
int  fwriteInt( int fh, int val)
{
    FILE *  pFile = (FILE *)fh;
    char*   pc = (char*)&val;
    fwrite(pc,1,4,pFile);
    return 0;
}

//-- ������ ������ ����� �� ��������� �����
int  freadInt( int fh )
{
    FILE *  pFile = (FILE *)fh;
    int     val;
    char*   pc = (char*)&val;
    fread(pc,1,4,pFile);
    return val;
}
//== ��������� ����� ����� � ������ ========================
int  countTextSt(char* pc)
{
    if( !pc ) return - BAD_PTR;
    if( *pc == 0) return 0;
    int     res = 1;
    for( ; *pc; pc++ )
    	if( *pc == '\n') res++;
    //-- ��������� ��������� ������
    if( *(pc-1) == '\n') res--;
    return res;
}

//== �������� ������ � �������� ������� �� ������ ==========
//-- ���� ������� �� 0
int  getTextSt(char*  pc, char* pcres, int res_size,  int st_nom)
{
    if( !pc || !pcres ) return - BAD_PTR;
    int  st_count = countTextSt( pc );
    if(( st_nom < 0)||(st_nom >= st_count)) {
        *pcres = 0;    return - BAD_NUMBER;
    }
    int   st_beg;
    char  *pc_dst = pcres;
    //-- ���� ������ ������ ������
    for( ; st_nom > 0; pc++ )
        if( *pc == '\n' ) st_nom--;
    //-- �������� ��������� ������
    for( ; (*pc)&&( *pc != '\n');  ){
        if( pc_dst - pcres >= res_size - 1) {
            *pc_dst = 0;  return  - BUFFER_SMALL;
        };
        *pc_dst++ = *pc++;
    }
    *pc_dst = 0;
    return  pc_dst - pcres - 1;
}
//-------------------------------------------------
//-- ����� �������
//-- ���� ��� ��������
char* trim( char* pc, char* pcres)
{   //-- ���� ������
    while( (*pc)&&((*pc == ' ')||(*pc =='\t')))
        pc++;
    if( !*pc )  //-- ��� ������ ����� ��� ������
    {   *pcres = 0;  return pcres;
    }
    //-- ���� ����� �������� ������
    char*  pc_end = pc;
    while( *pc_end)  pc_end++;
    //-- ���� ����� �� �����
    pc_end--;
    while( (*pc_end == ' ')||(*pc_end == '\t'))
        pc_end--;
    //--�������� ����� ������
    char*  pc_dst = pcres;
    for( pc_end++;  pc != pc_end; )
        *pc_dst++ = *pc++;
    *pc_dst =0;
    return pcres;
}
//----------------------------------------------------------
//== ����� �������� � ����� ====
//== *** beg *** ==
//== ������� ������ ������� ���������� �� ����� �����
//-- ���������� 0 ��� ������ (����� ������� � ������� )
//-- ��� ������������� ��� ������
int createDBSN( char*  fil_name)
{
    char*   pc_buff = new char[ getLen( fil_name )+1];
    int     err_cod = 0;
    fil_name = trim(fil_name, pc_buff);
    if(getLen(fil_name) == 0){  //-- ��������� ���������� ���
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fh = (int)fopen(fil_dbs, "w");
    if(!fh) {               //-- �� ������ ������� ���� dbs
        err_cod = DBS_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fh);
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    fh = (int)  fopen(fil_dbs, "wb");
    if(!fh){               //-- �� ������ ������� ���� dbn
        err_cod = DBN_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fh);
    return 0;
}
//----------------------------------------------------------

//== ��������� ������������� ������ DBSN
//-- ���������� 1 ��� ������
//-- ��� ������������� ��� ������
int  existDBSN( char* fil_name ) {
    int     err_cod = 0;
    char*  pc_buff = new char[ getLen( fil_name )+1];
    fil_name = trim(fil_name, pc_buff);
    if( getLen(fil_name) == 0){  //-- ��������� ���������� ���
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fhs=(int)fopen(fil_dbs,"rb");
    if(!fhs){                   //-- �� ������ ������� ���� dbs
        err_cod = DBS_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fhs);
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    int     fhn=(int)fopen(fil_dbs,"rb");
    if(!fhn){                   //-- �� ������ ������� ���� dbn
        err_cod = DBN_NOT_EXIST;
        return -err_cod;
    }
    fclose((FILE*)fhn);
    return 1;
};
//----------------------------------------------------------

//== �������� ��� ���������� DBS �����
//-- ���������� ����� ����� ��� ������
//-- ��� ������������� ��� ������
int getAllDBS( char*  fil_name, char* &text ) {
    int     res = existDBSN( fil_name);
    if( res <= 0){   //-- ������� �� ����������
        *text = 0;  return  res;
    }
        //-- ��������� ��������� ����
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fhl = (int) fopen(fil_dbs,"rb");
    fseek((FILE*)fhl,0,2);
    int  file_size = ftell((FILE*)fhl);
        //-- ������� ����� � ������
    text  =  new char[ file_size +1 ];
    void *pv =  (void*)text;
    if( !pv ){  //-- �� ������� ��������� ������
        res = BUFFER_SMALL;  return -res;
    }
        //-- ��������� � ���� ���� ����
    fread( pv, 1,1,(FILE*)fhl);
    fclose((FILE*)fhl);
    return file_size;
}
//----------------------------------------------------------

//== ������� ������� ���������� DBSN
//--  ���������� ���������� ������� ��� ������
//--  (����� ������� ���������� )
//--  ��� ������������� ��� ������
int openDBSNA( char* fil_name ) {
    int     res = existDBSN( fil_name );
    if( res < 0 ) return  res;

    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    int     fhn = (int)fopen(fil_dbs, "rb+");
    int     dbhadr =  getLongArray( 4 );
    int*    dbh = (int*) dbhadr;
        //-- ��������� ���������� �������
    dbh[0] = fhn;       //-- ���������� ��������� �����
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    int     fhs = (int)fopen(fil_dbs, "rb+");
    dbh[1] = fhs;       //-- ���������� ���������� �����
    int     rec_nom = 0;
    dbh[2] = rec_nom;   //-- ����� ������� ������
    dbh[3] = 0;         //-- ��� ������
    return  dbhadr;
}
//----------------------------------------------------------

//== ������� ������� ���������� DBSN
//-- ���������� 0 ��� ���������� ��� ��������� dbhadr
//-- ��� ������������� ��� ��������� ������
//-- ����������� ������ �������������
//      dbhadr  =  closeDBSN( dbhadr );
//-- ����� ����� ����� �������� ������������  dbhadr
int  closeDBSN( int &dbhadr ) {
    int     err_cod = 0;
    if( !dbhadr ){  //-- �� ����� ����������
        err_cod = BAD_TABLE_HANDLE;  return -err_cod;
    }
    int*  dbh = (int*) dbhadr;
    err_cod = dbh[3];
    if( err_cod >= FATAL_ERROR)  return -err_cod;
    int     fhn = dbh[0];
    fflush( (FILE*)fhn );
    fclose((FILE*)fhn);
    int     fhs = dbh[1];
    fflush( (FILE*)fhs );
    fclose((FILE*)fhs);
    delete[] dbh;
    dbhadr = 0;
    return 0;
}
//----------------------------------------------------------

//== ������� ��� ��������� ������� DBSN
//--  ������ ������ �� ����� ��������������
//--  ���������� ���������� ������� � �������
//-- (��������� � ������� ��������� ������)
//--  ��� ������������� ��� ��������� ������
int  delAllFragm(  int dbhadr ) {
    int     err_cod = 0;
    if( !dbhadr ){  //-- �� ����� ����������
        err_cod = BAD_TABLE_HANDLE;  return -err_cod;
    }
    int*  dbh = (int*) dbhadr;
    err_cod = dbh[3];
    if( err_cod >= FATAL_ERROR )  return -err_cod;

    dbh[3] = 0; //-- ������� ��������������
    //-- ���������� ���������� �������
    int     fhn = dbh[0];
    fseek( (FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int     rec_size = 0;
    //-- � ����� ������� ��� ������
    for( int  rec_nom = 1; rec_nom <= rec_count; rec_nom++)
    {	//-- ��������� ���������� ��������� ���������
        fseek( (FILE*)fhn, (rec_nom-1)*8 + 4, 0);
        rec_size = freadInt( fhn );
        if(rec_size > 0)
        {	rec_size = -rec_size;
            fseek( (FILE*)fhn, (rec_nom-1)*8 + 4, 0);
            fwriteInt( fhn, rec_size );
        }
    }
    dbh[2] = rec_count;
    return rec_count;
};
//----------------------------------------------------------

//== ��������� ����������  ������� DBSN
//-- ���������� 1 ��� ������
//-- ��� ������������� ��� ������
int  eraseDBSN( char*  fil_name ) {
    int     err_cod = 0;
    char*  pc_buff = new char[ getLen( fil_name )+1];
    fil_name = trim(fil_name, pc_buff);
    if( getLen(fil_name) == 0){  //-- ��������� ���������� ���
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  fil_dbs = new char[ getLen( fil_name )+ 5];
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbs");
    remove(fil_dbs);
    strcpy( fil_dbs, fil_name);
    strcat( fil_dbs, ".dbn");
    remove(fil_dbs);
    return 1;
}
//----------------------------------------------------------

//==  ������������� ������� DBSN � ����� ���
//--  ���������� 1 ��� ������
//--  ��� ������������� ��� ������
int  renameDBSN( char* old_fil, char* new_fil ) {
    int     err_cod = 0;
    if(!existDBSN(old_fil)) {
        err_cod = BAD_TABLE_NAME;
        return -err_cod;
    }
    char*  oldfil_dbs = new char[ getLen( old_fil )+ 5];
    strcpy( oldfil_dbs, old_fil);
    strcat( oldfil_dbs, ".dbs");
    char*  newfil_dbs = new char[ getLen( new_fil )+ 5];
    strcpy( newfil_dbs, new_fil );
    strcat( newfil_dbs, ".dbs" );
    rename( oldfil_dbs, newfil_dbs);

    strcpy( oldfil_dbs, old_fil);
    strcat( oldfil_dbs, ".dbn");
    strcpy( newfil_dbs, new_fil );
    strcat( newfil_dbs, ".dbn" );
    rename( oldfil_dbs, newfil_dbs);

    return 1;
};
//----------------------------------------------------------

//== ��������� �� ����� ����� ������� DBSN
//--  ���������� 1 ���� ������� ��� ��� ����� �� �������
//--  ��� ������������� ��� ������
int  isEndDBSN( int dbhadr ) {
    int     err_cod = 0;
    if( !dbhadr ){  //-- �� ����� ����������
        err_cod = BAD_TABLE_HANDLE;  return -err_cod;
    }

    int*    dbh = (int*) dbhadr;
    err_cod = dbh[3];
    if( err_cod >= FATAL_ERROR )  return -err_cod;

   	if( dbh[3] & OUT_OF_RANGE )
		return 1;
	if( dbh[3] & DBN_IS_EMPTY )
		return 1;
	return 0;
}
//----------------------------------------------------------
//== *** end *** ==
//==========================================================

//== ������ � ������� ����������  ==========================
//-- ��������� ������ �� 1 ��  countNblocks ������������

//----------------------------------------------------------
//== 07 apr 2013 == ������ � ��������� ����������� =========
//== 20 ��� 2014 ====
//-- ������ �������� �������� ������� �� �������� �������� =
//-- Binsubst  ����:   ���������������...���[000]   ���
//--    ���� - �������������� ����� ������(��������� �����),
//--    ������...�� - �� ���������� (������������ �����)
//--    ���� ����� ��������� �� ������ 4 ��� ����������� 0-�� � �����
//--    len -  ����� � ������(��� ��������� ��������� ������ ������ 4
//--    size - ������ � ������ ( size = len /4)
//==========================================================

//-- ��������� ����� ���������� �������� ��������� � ������
int  binsubstLenSh( char* pc ) {
    return  *(int*) pc;
}
//----------------------------------------------------------

//-- ��������� ����� ���������� �������� ���������� � ������
int  binsubstSizeSh( char* pc ) {
    int     loc_size = 4 + binsubstLenSh(  pc );
    loc_size = ( loc_size + 3 ) /4;  //-- ����� ������� ����
    return  loc_size;
}
//----------------------------------------------------------

//-- ��������� ����� �������� � �������� ������
//-- ���������� ������� ��� �������� ������������ ����������
int binsubstCountIn( char* pcFragm, int fragmLen) {
    int     count = 0, offset = 0;
    while( offset < fragmLen ) {
        count++;
        offset +=  binsubstSizeSh( pcFragm + offset )*4;
    }
    return count;
};
//----------------------------------------------------------
//-- ��������� ����� �������� � �������� ������
int binsubstCount( char* pcFragm, int fragmLen) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR ;
    return binsubstCountIn( pcFragm, fragmLen);
};
//----------------------------------------------------------

//== �������� �������� �������� ��������� � ������
//-- pcFragm - �������� �������� � �����������
//-- fragmLen - ��� ����� ��� �������� ������
//-- nom_sub - ����� ��������� �������� ���������
//-- ��� ������ ���������� -BAD_NUMBER
//-- ���������� ������� ��� �������� ������������ ����������
int   binsubstOffsetIn(char* pcFragm, int fragmLen, long nom_sub) {
    int     count = 0, offset = 0;
    while(( offset < fragmLen )&&( count < nom_sub )) {
        count++;   offset +=  binsubstSizeSh( pcFragm + offset )*4;
    }
    return  offset;  //-- �������� �������� ���������
}

//-- ������� ������� �������  --
int   binsubstOffset(char* pcFragm, int fragmLen, long nom_sub) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
}
//----------------------------------------------------------

//== �������� ��������� �� �������� ��������� � �������� ������� �� ������
//-- ���������� 0 ��� ������� ������� ������� ��� ������� ���������
//-- ���������� ������� ��� �������� ������������ ����������
char* getBinsubstPtIn(char* pcFragm, int fragmLen, long nom_sub) {
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    return  pcFragm + offset;
}

//-- ������� ������� �������  --
char* getBinsubstPt(char* pcFragm, int fragmLen, long nom_sub) {
    if (( !pcFragm )||( !fragmLen))  return  0;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return 0;
    return  getBinsubstPtIn( pcFragm, fragmLen, nom_sub);
}
//----------------------------------------------------------

//== ��������� ����� ��������� �� �� ������ ================
//-- ���������� ������� ��� �������� ������������ ����������
int  binsubstLenIn( char* pcFragm, int fragmLen, long nom_sub ) {
    return  binsubstLenSh( getBinsubstPtIn( pcFragm, fragmLen, nom_sub ) );
}

//-- ������� ������� �������  --
int  binsubstLen( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  binsubstLenIn(  pcFragm, fragmLen, nom_sub );
}
//----------------------------------------------------------

//== ��������� ������ ��������� � ������ �� �� ������ ======
//-- ���������� ������� ��� �������� ������������ ����������
int  binsubstSizeIn( char* pcFragm, int fragmLen, long nom_sub ) {
    return  binsubstSizeSh( getBinsubstPtIn( pcFragm, fragmLen, nom_sub ) );
}

//-- ������� ������� �������  --
int  binsubstSize( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  binsubstSizeIn(  pcFragm, fragmLen, nom_sub );
}
//----------------------------------------------------------

//-- ����������� �������� ��������� � �������� ������� �� ������ � �����
//-- ���������� ������� ��� �������� ������� ������
//-- ���������� ����� ������������� ���� (����� �������� ��������� + 4)
int  getBinsubstIn(char* pcFragm, int fragmLen, char* pcBuf, long nom_sub)
{   //-- ���� ������ �������� ��������� � �� �����
    char*  pcBinSubst = getBinsubstPtIn( pcFragm, fragmLen, nom_sub);
    int     loc_len = binsubstSizeSh( pcBinSubst )*4;
    for( int  loc_nom = 0; loc_nom < loc_len; loc_nom++ ) {
        *pcBuf++ = *pcBinSubst++;
    }
    return  loc_len;
};

//== ������� ������� ������� � ����������
int  getBinsubst(char* pcFragm, int fragmLen, char* pcBuf, int buf_size, long nom_sub){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( buf_size < binsubstSizeIn( pcFragm, fragmLen, nom_sub )*4 ) return  -BUFFER_SMALL;
    return  getBinsubstIn( pcFragm, fragmLen, pcBuf, nom_sub);
}

//----------------------------------------------------------

//== �������� � ��������� ��������� ========================
//-- ������� ����� pcBuf ��������� BufLen
//-- � ���� ���������� �������� � � ����� �������������� �������� ��������� pcBinsubst
//-- ������������ ����� ����� ���������
//-- ��� ������ ���������� ������������� ���
//-- ���������� �������
int addBinsubstIn( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, char* pcBinsubst ) {
    int  byte_nom = 0;
    for( byte_nom = 0; byte_nom < fragmLen; byte_nom++ )
        *pcBuf++ = *pcFragm++;
    int     count = binsubstSizeSh( pcBinsubst )*4;
    for( byte_nom = 0; byte_nom < count; byte_nom++ )
        *pcBuf++ = *pcBinsubst++;
    return  count + fragmLen;
}

//== ������� ������� =======================================
int addBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen,  char* pcBinSubst ) {
    if (( !pcFragm )||( !pcBinSubst))  return - BAD_PTR;
    if( bufLen-fragmLen < binsubstSizeSh( pcBinSubst )*4 ) return  -BUFFER_SMALL;
    return  addBinsubstIn( pcFragm, fragmLen, pcBuf, bufLen, pcBinSubst );
}
//----------------------------------------------------------

//== ������� �� ��������� ��������� ========================
//-- ������� ����� pcFragm
//-- �� ��� ��������� �������� ��������� � �������� �������
//-- ������������ ����� ����� ���������
//-- ���������� ������� ��� �������� �� ������
int cutBinsubstIn( char* pcFragm, int fragmLen, long nom_sub ) {
    int  offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int  loc_len = binsubstSizeSh( pcFragm + offset )*4;
    for( int  byte_nom = offset; byte_nom < fragmLen - loc_len; byte_nom++ )
        pcFragm[ byte_nom ] = pcFragm[ byte_nom + loc_len ] ;
    return  fragmLen - loc_len;
}

//== ������� ������� c ���������� ������
//-- ��� ������ ����������  ������������� ���
int cutBinsubst( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  cutBinsubstIn(  pcFragm,  fragmLen, nom_sub );
}
//----------------------------------------------------------

//== �������� ���������  � ��������� ========================
//-- ������� ����� pcFragm
//-- �������� ��������� � �������� ������� � ��� ����������
//-- ������������ ����� ����� ���������
//-- ���������� ������� ��� �������� �� ������
int clearBinsubstIn( char* pcFragm, int fragmLen, long nom_sub ) {
    int  offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int  loc_len = binsubstSizeSh( pcFragm + offset )*4;
    int*    pl = (int*) (pcFragm+offset);   *pl = 0;  //-- ������� �����
    loc_len -= 4;
    for( int  byte_nom = offset+4; byte_nom < fragmLen - loc_len; byte_nom++ )
        pcFragm[ byte_nom ] = pcFragm[ byte_nom + loc_len ] ;
    return  fragmLen - loc_len;
}

//== ������� ������� c ���������� ������
//-- ��� ������ ����������  ������������� ���
int clearBinsubst( char* pcFragm, int fragmLen, long nom_sub ) {
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  clearBinsubstIn(  pcFragm,  fragmLen, nom_sub );
}
//----------------------------------------------------------

//== �������� ��������� ����� �������� �� ��������
//-- ������� ����� pcBuf ��������� bufLen
//-- � ���� ���������� �������� �� ����������� � ���� ����� ����������
//-- ������������ ����� ����� ���������

//-- ���������� ������� ��� �������� �� ������
int insBinsubstIn( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst ){
    int     loc_len = binsubstSizeSh( pcBinsubst )*4;
    int     byte_nom = 0;
        //-- �������� � ����� ��������� ����� ���������
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    for( byte_nom = 0; byte_nom < offset; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- ��������� � ����� ����� ���������
    for( byte_nom = 0; byte_nom < loc_len; byte_nom++ ) {
            *pcBuf++ = *pcBinsubst++;
    }
        //-- ������������ ������� ���������
    for( byte_nom = offset; byte_nom < fragmLen; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- ���������� ����� ����� ���������
    return  loc_len + fragmLen;
}

//== ������� ������� c ���������� ������
//-- ��� ������ ����������  ������������� ���
int insBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst  ){
    if (( !pcFragm )||( !pcBinsubst))  return - BAD_PTR;
    if( bufLen-fragmLen < binsubstSizeSh( pcBinsubst )*4 ) return  -BUFFER_SMALL;
    if( nom_sub >= binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  insBinsubstIn(  pcFragm, fragmLen, pcBuf, bufLen, nom_sub, pcBinsubst );
}

//----------------------------------------------------------

//== �������� ��������� ��������� �� �����
//-- ������� ����� pcBuf ��������� bufLen
//-- � ���� ���������� �������� � ���������� �� ����� �������� ����������
//-- ������������ ����� ����� ���������

//-- ���������� ������� ��� �������� �� ������
int setBinsubstIn( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst ){
    int     loc_len = binsubstSizeSh( pcBinsubst )*4;
    int     old_len = binsubstSizeIn( pcFragm, fragmLen, nom_sub )*4;
    int     byte_nom = 0;
        //-- �������� � ����� ��������� ����� ���������
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    for( byte_nom = 0; byte_nom < offset; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- ��������� � ����� ����� ���������
    for( byte_nom = 0; byte_nom < loc_len; byte_nom++ ) {
            *pcBuf++ = *pcBinsubst++;
    }
        //-- ������������ ������� ���������
    for( byte_nom = offset + old_len, pcFragm += old_len; byte_nom < fragmLen; byte_nom++ ) {
            *pcBuf++ = *pcFragm++;
    }
        //-- ���������� ����� ����� ���������
    return  loc_len - old_len + fragmLen;
}

//== ������� ������� c ���������� ������
//-- ��� ������ ����������  ������������� ���
int setBinsubst( char* pcFragm, int fragmLen, char* pcBuf, int bufLen, long nom_sub, char* pcBinsubst){
    if (( !pcFragm )||( !pcBinsubst))  return - BAD_PTR;
    if( bufLen-fragmLen < binsubstSizeSh( pcBinsubst )*4 ) return  -BUFFER_SMALL;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  setBinsubstIn( pcFragm, fragmLen, pcBuf, bufLen, nom_sub,  pcBinsubst );
}
//----------------------------------------------------------

//== 04 aug 2013 ==== active Records  ======================

//-- �������� 2 ������ � ������ �������� � ������������� ��������
//-- ������� ����������, ����������
//-- 1  ���� *pc > *pc1
//-- 0  ���� *pc == *pc1
//-- -1 ���� *pc < *pc1
//-- ��� ��������� ������� �������� "|" ������� ������ ������ ����
//-- ��������������� �� 0 ����� �� �����
long    compSpec( char* pc, char* pc1){
    unsigned  char  bt, bt1;
    while( 1 ){
        bt = *pc++;  bt1 = *pc1++;
        //-- ������������ ����� �����
        if(( bt == 0 )&&( bt1 == 0 ))  return 0;
        if( bt == 0 )   return -1;
        if( bt1 == 0 )  return  1;
        //-- ������������ ����� ��������
        if(( bt == '|' )&&( bt1 == '|' ))  continue;
        if( bt == '|' )   return -1;
        if( bt1 == '|' )  return  1;
        //-- ��������� ��� ����� ��������
        bt = toupper(bt);  bt1 = toupper(bt1);
        if( bt == bt1 )  continue;
        if( bt > bt1 )   return  1;
        if( bt < bt1 )   return -1;
    }
};
//----------------------------------------------------------


//== 09 aug 2013 ===========================================
//-- ������ 2� ����� ����� � �������� ����
int* writeBinRec( int  dbhadr, int* plbuf)
{
    int*    dbh = (int*) dbhadr;
    if( dbh[3] > FATAL_ERROR )  return 0;
    int     fhn = dbh[0];
    int     rec_nom = dbh[2];
    FILE *  pFile = (FILE *)fhn;
    char*   pc = (char*)plbuf;
    fseek(  pFile, (  rec_nom-1)*8, 0);
    fwrite(pc,1,8,pFile);
    return plbuf;
}
//----------------------------------------------------------

//-- ������ 2� ����� ����� �� ��������� �����
int* readBinRec( int  dbhadr, int* plbuf )
{
    int*    dbh = (int*) dbhadr;
    if( dbh[3] > FATAL_ERROR )  return 0;
    int     fhn = dbh[0];
    int     rec_nom = dbh[2];
    FILE *  pFile = (FILE *)fhn;
    fseek(  pFile, (  rec_nom-1)*8, 0);
    char*   pc = (char*)plbuf;
    fread(pc,1,8,pFile);
    return  plbuf;
}
//----------------------------------------------------------

//== ����� ������� ���� ���������� (� ��������� �����)
//--  ������ ������� ������� � �������� ������ � �������� �����
//--  ���������� ����� ������ dest_nom ��� 0 ��� ������
//--  ���� ���������� � dest_nom
int  xchgFragm( int  dbhadr, int  dest_nom ) {
        //-- ��������� ����� ��������
    if( !dbhadr )  return 0;
    int     cur_nom = getNom( dbhadr );
    if( cur_nom == dest_nom )  return cur_nom;
        //-- ��������� 8 ������ �������� ������
    int     cur_rec[2],
            dest_rec[2],
            tmp;
    readBinRec( dbhadr, cur_rec );
        //-- ��������� ����� ����������
    if( dest_nom < 1) return 0;
    int     rec_count = countFragmA( dbhadr );
    if( dest_nom > rec_count ) return 0;
        //-- ��������� 8 ������ ��������� ������
    setNom( dbhadr, dest_nom );  readBinRec( dbhadr, dest_rec );
        //-- ������ ������� ������ �������
    for(int j = 0; j<2; j++) {
        tmp = cur_rec[j];  cur_rec[j] = dest_rec[j];  dest_rec[j] = tmp;
    }
        //-- ���������� ���������� � �������� ����
    setNom( dbhadr, cur_nom );  writeBinRec( dbhadr, cur_rec );
    setNom( dbhadr, dest_nom );  writeBinRec( dbhadr, dest_rec );
    return   dest_nom;
};
//----------------------------------------------------------

//== ������� �� ���� ��� ������ �������
int    flushDBSN( int dbhadr ){
    int*    dbh = (int*) dbhadr;
    int     fhn =   dbh[0];
    if( !fhn ) return 0;
    fflush( (FILE*)fhn );
    fhn =   dbh[1];
    if( !fhn ) return 0;
    fflush( (FILE*)fhn );
    return 1;
};
//----------------------------------------------------------

//== ��������� �������� ������ ���������  �� dbn
//-- ���������� ����� ���������� ��������� ��� ����������
//-- ��� 0 ��� ������
int    cutFragm(  int dbhadr ){
    int*    dbh = (int*) dbhadr;
    int     fhn =   dbh[0];
    if( !fhn ) return 0;
    if( dbh[3] > FATAL_ERROR )  return 0;
    dbh[3] = 0; //-- ������� ��������������

    int     fragm_nom = dbh[2];  //-- ��� ����� ���������
    int     file_off = (fragm_nom-1)*8; //-- �������� ���������� ���������
    int     clast_nom = file_off/ 4096; //-- ����� �������� � ���
    fseek((FILE*)fhn,0,2);
    int     file_len =  ftell( (FILE*)fhn );

    if( !file_len )
        dbh[3] = DBN_IS_EMPTY;
    if( file_len < file_off ) return 0;
    int     fragm_count = file_len /8;

    int     clast_count = (file_len + 4095)/4096;   //-- ����� ��������� � �������� �����
    char    buff[ 8192 ];      //-- ����� ��� 2� ���������
    int     cur_offset = file_off - clast_nom * 4096; //-- ���� � ��������
    //-- ��������� 2 ��������� � �����
    fseek((FILE*)fhn, clast_nom*4096 ,0);
    int     read_len =  fread((void*) buff, 1, 8192, (FILE*)fhn);
    int     nom = 0;
    //-- �������� �� 8 ����� �����
    for( nom = cur_offset; nom < read_len -8; nom++)
        buff[ nom ] = buff[ nom +8 ];
    if( clast_nom >= clast_count - 2 ) {  //-- �������� ����
        fseek((FILE*)fhn, clast_nom*4096 ,0);
        fwrite((void*) buff, read_len - 8, 1, (FILE*)fhn);
    }else{  //-- ���� ���������� ����� �� ������� � �����
        //-- ���������� ������ �������
        fseek((FILE*)fhn, clast_nom*4096 ,0);
        fwrite((void*) buff, 4096, 1, (FILE*)fhn);
        clast_nom +=1;
        while( clast_nom < clast_count-1 ){
            //-- �������� �� ��� ����� ������
            for( nom = 0; nom < 4096; nom++)
            buff[ nom ] = buff[ nom +4096 ];
            //-- ���������� � ����� ������ ��������� �������
            fseek((FILE*)fhn, (clast_nom+1)*4096 ,0);
            read_len =  fread((void*) (buff+4096), 1, 4096, (FILE*)fhn);
            //-- �������� ������ ������� ����� �� 8 ����
            for( nom = 4096; nom < 8192; nom++)
                buff[ nom-8 ] = buff[ nom ];
            //-- ���������� ������ �������� ������ �� ����
            fseek((FILE*)fhn, clast_nom*4096 ,0);
            fwrite((void*) buff, 4096, 1, (FILE*)fhn);
            clast_nom++;
        }
        //-- ���������� ��������� ����� � �����
        if(read_len > 8){
            fseek((FILE*)fhn, clast_nom*4096 ,0);
            fwrite((void*) buff, read_len-8, 1, (FILE*)fhn);
        }
    }
    if( fragm_nom >= fragm_count ) fragm_nom --;
    dbh[2] = fragm_nom;
    ftruncate(fileno( (FILE*)fhn ), file_len - 8);
    return  fragm_nom;
};
//----------------------------------------------------------


//== ����������� ������� ��������� � �������� ����� dbn
//--  ���������� dest_nom
//--  ��� 0 ��� ������
int    moveFragm(  int dbhadr, int  dest_nom ) {
    //-- ����������� �������� �� ������
    int*    dbh = (int*) dbhadr;
    int     fhn =   dbh[0];
    if( !fhn )  return 0;
    if( dbh[3] > FATAL_ERROR )  return 0;
    dbh[3] = 0; //-- ������� ��������������

    int     curr_nom = dbh[2];  //-- ��� ����� ���������
    if( dest_nom == curr_nom )  //-- ����������� �� �����
        return  curr_nom;

    fseek((FILE*)fhn,0,2);
    int     file_len =  ftell( (FILE*)fhn );
    if( !file_len ){ //-- ������ �� ������ ����
        dbh[3] = DBN_IS_EMPTY;
        return 0;
    }

    int     fragm_count = file_len /8;
    if( ( dest_nom < 1 )||( dest_nom > fragm_count) )
        return  0;  //-- ������ �� ������� ��������

    //-- ���������������� ��������
    int     curr_off   = (curr_nom-1)*8; //-- �������� ��������� ���������
    int     curr_clast = curr_off/ 4096; //-- ����� �������� � ���
    int     loc_curroff    = curr_off % 4096; //-- ��������� �������� ��� �����
    int     dest_off   = (dest_nom-1)*8; //-- �������� ������� �������
    int     dest_clast = dest_off/ 4096; //-- ����� �������� � ���
    int     loc_destoff    = dest_off % 4096; //-- ��������� �������� ������� �������

    char    buff[ 8192 ];      //-- ����� ��� 2� ���������
    char    curr_rec[8];      //-- ����� ��� ������� ������
    int     byte_nom;
    int     begin_pos;
    int     write_len = 0;
    int     clast_nom;

    //-- ���������� ���������� ������
    fseek((FILE*)fhn, curr_clast*4096 ,0);
    int    read_len =  fread((void*) buff, 1, 4096, (FILE*)fhn);
    for( byte_nom = 0; byte_nom < 8; byte_nom++)
        curr_rec[byte_nom] = buff[ loc_curroff + byte_nom ];

    //-- ���������� ������
    if( dest_nom > curr_nom ) { //-- ��������� ����� ��������� ������
        if(  curr_clast == dest_clast ){ //--� ����� ��������
            //-- �������� ���������� �����
            for( byte_nom = loc_curroff; byte_nom < loc_destoff + 8; byte_nom++)
                buff[byte_nom] = buff[ byte_nom + 8 ];
            //-- ���������� �������� ������ � ����� �����
            for( byte_nom = 0; byte_nom < 8; byte_nom++)
                buff[ byte_nom + loc_destoff ] = curr_rec[byte_nom];
            //-- ��������� ������� �� �����
            fseek((FILE*)fhn, curr_clast*4096 ,0);
            write_len =  fwrite((void*) buff, 1, read_len, (FILE*)fhn);
        } else { //-- � ������ ���������
            //-- �������� ���������� �����
            for( byte_nom = loc_curroff; byte_nom < 4096; byte_nom++)
                buff[byte_nom] = buff[ byte_nom + 8 ];
            while ( 1 ){ //-- �������� ����� ��� ������������� ��������
                write_len = read_len;
                //-- ��������� ���������� �������
                curr_clast++;
                fseek((FILE*)fhn, curr_clast*4096 ,0);
                read_len =  fread((void*) (buff + 4096 ), 1, 4096, (FILE*)fhn);
                if(  curr_clast != dest_clast) { //-- �� ������� �������
                    //-- �������� ��� �����
                    for( byte_nom = 4096; byte_nom < 8192; byte_nom++)
                        buff[byte_nom - 8] = buff[ byte_nom ];
                    //-- ���������� ������ �������� ������
                    fseek((FILE*)fhn, ( curr_clast + 1)*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, 4096, (FILE*)fhn);
                } else { //-- ��������� �� �������� ��������
                    //-- �������� ��� �����
                    for( byte_nom = 4096; byte_nom < 4096 +loc_destoff + 8; byte_nom++)
                        buff[byte_nom - 8] = buff[ byte_nom ];
                    //-- ���������� �������� ������ � ����� �����
                    for( byte_nom = 0; byte_nom < 8; byte_nom++)
                        buff[ byte_nom + 4096 + loc_destoff ] = curr_rec[byte_nom];
                    //-- ���������� ���� ����� �� ����
                    fseek((FILE*)fhn, curr_clast*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, 4096 + read_len, (FILE*)fhn);
                    break;
                }
            }
        }
    }  else {//-- ��������� ����� ��������� �����
        if(  curr_clast == dest_clast ) { //--� ����� ��������
            //-- �������� ���������� ������
            for( byte_nom = loc_curroff - 1; byte_nom >= loc_destoff; byte_nom--)
                buff[byte_nom + 8] = buff[ byte_nom ];
            //-- ���������� �������� ������ � ����� �����
            for( byte_nom = 0; byte_nom < 8; byte_nom++)
                buff[ byte_nom + loc_destoff ] = curr_rec[byte_nom];
            //-- ��������� ������� �� �����
            fseek((FILE*)fhn, curr_clast*4096 ,0);
            write_len =  fwrite((void*) buff, 1, read_len, (FILE*)fhn);
        } else { //-- � ������ ���������
            //-- �������� ���������� ������
            for( byte_nom = loc_curroff - 1; byte_nom >= 0; byte_nom--)
                buff[byte_nom + 8] = buff[ byte_nom ];
            while ( 1 ){ //-- �������� ������ ��� ������������� ��������
                //-- ��������� ����������� �� ������ �������� ������
                for( byte_nom = 0; byte_nom < 4096; byte_nom++)
                buff[byte_nom + 4096] = buff[ byte_nom ];
                write_len = read_len;
                //-- ��������� ���������� �������
                curr_clast--;
                fseek((FILE*)fhn, curr_clast*4096 ,0);
                read_len =  fread((void*) buff, 1, 4096, (FILE*)fhn);

                if(  curr_clast != dest_clast) { //-- �� ������� �������
                    //-- �������� ��� ������
                    for( byte_nom = 4095; byte_nom >= 0; byte_nom--)
                        buff[byte_nom + 8] = buff[ byte_nom ];
                    //-- ���������� ������ �������� ������
                    fseek((FILE*)fhn, ( curr_clast + 1)*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, write_len, (FILE*)fhn);
                } else { //-- ��������� �� �������� ��������
                    //-- �������� ��� ������
                    for( byte_nom = 4095; byte_nom >= loc_destoff; byte_nom--)
                        buff[byte_nom + 8] = buff[ byte_nom ];
                    //-- ���������� �������� ������ � ����� �����
                    for( byte_nom = 0; byte_nom < 8; byte_nom++)
                        buff[ byte_nom + loc_destoff ] = curr_rec[byte_nom];
                    //-- ���������� ���� ����� �� ����
                    fseek((FILE*)fhn, curr_clast*4096 ,0);
                    write_len =  fwrite((void*) buff, 1, write_len + read_len, (FILE*)fhn);
                    break;
                }
            }
        }
    }
    dbh[2] = dest_nom;
    return  dest_nom;
};
//----------------------------------------------------------

//==  �������� ������ � ���� ���������� DBSN � ������� �������
int    insFragm(  int dbhadr,  char* fragm){
        //-- �������� ����� ������� ������
    int     dest_nom = getNom( dbhadr );
        //-- ��������� � ������� ����� ��������
    addFragm( dbhadr, fragm );
        //-- ���������� ��� � ������ ������� �������
    dest_nom = moveFragm(  dbhadr, dest_nom );
    return  dest_nom;
};
//----------------------------------------------------------


//== ����������� ���� ���������� ��� ������ ������
//--  ���������� ���������� ������������ �������
//--  ��� 0 ��� ������
//-- ���  flag_pack != 0 ��������� ������ �� ����������

int    copyDBSN( char*  fil_name, char*  copy_filname, int flag_pack ) {
    int     res,
            new_count = 0;  //-- ����� ������������ �������
    int     rec_size = 0,
            max_recsize = 0,
            rec_nom,
            rec_count;
    int     dbhadr = 0,
            dbhnewadr = 0;

    //== ����������� ����� ������ ���������
    dbhadr = openDBSNA( fil_name );
    if( !dbhadr )   return 0;
    int*    dbh = ( int* )dbhadr;
    if( dbh[3] > FATAL_ERROR)    return 0;
    dbh[3] = 0; //-- ������� ��������������
        //-- ���������� �������� ���������� �������
    rec_count = countFragmA( dbhadr );
    if( rec_count == 0 )  {
        closeDBSN( dbhadr );
        return 0;
    }
        //-- �������� �� ���� �������
    for( rec_nom = 1; rec_nom <= rec_count; rec_nom++)  {
        setNom( dbhadr, rec_nom );
        res = getFragmSize(dbhadr, rec_size);
        if( rec_size < 0 )  rec_size = - rec_size;
        if( max_recsize < rec_size ) max_recsize = rec_size;
    }
        //-- ��������� ����� ���������
    int     buf_size = max_recsize + 16;
    char    *fragm;
    fragm   = new char[buf_size];

    //== �������� �������� ���� � ��������������
        //-- ��������� �������������� ����
    res = createDBSN( copy_filname );
    if( !res ) {
        closeDBSN( dbhadr );
        return 0;
    }

    dbhnewadr = openDBSNA( copy_filname );
    if( !dbhnewadr )   return 0;

        //-- � ����� �������� ��� ����������� ������
    for( rec_nom = 1; rec_nom <= rec_count; rec_nom++)  {
        res = setNom( dbhadr, rec_nom );
        res = getFragmSize( dbhadr, rec_size );
        if(( res <= 0 )&&( flag_pack ))  continue;

        res = getFragm(  dbhadr, fragm, buf_size );
        res = addFragm(dbhnewadr, fragm );
        new_count++;
    };
    //== ��������� ���� � ��������� ������
    closeDBSN( dbhadr );
    closeDBSN( dbhnewadr );
    return  new_count;
};
//----------------------------------------------------------

//== ��������� ���� DBSN
//--  ��������� ������� ��������� � �������������� �������
//--  ���������� ����� ���������� �������
//--  ��� 0 ��� ������
int  packDBSN( char* fil_name ) {
        //-- ������ ��������� ��� ����� ����
    char    temp_fil[200];
    strcpy( temp_fil, tmpnam ( "0000" ));
        //-- �������� � ��� ������ � ������ ��������
    int     new_count = copyDBSN( fil_name, temp_fil, 1 );
    if( new_count ) {
        int     res = eraseDBSN(fil_name);
        if( !res )  return 0;
        res = renameDBSN(temp_fil, fil_name);
        if( !res ) return 0;
    }
    //-- ���������� ����� ������������� �������
    return  new_count;
};
//----------------------------------------------------------

//==  �������� � ����� �������� ������ � ������������
//--  ���������� ������� ������� ����������
//--  ��� ���������� � ������ �������� ���� ������������ ������
//--  � �� ������������, ������������ ���� � �������������
//--  ���������� ������ ��������� ����� ������
//--  ��� 0 ���� ������ �� ���� ��� ����� ������ ����������
//    ������ ���������� ��������� ���� ���������� �������
//    ���� ������������� ��� ������
//      err_buf - ����� � ������� ����������� ������ �� ������
//      buf_len - ����� ����� ������
//      comment - ��������� �����������
int  getErrMes( int dbhadr, char* err_buf, int buf_len, char* comment){
    if( !dbhadr )   return 0;
    int     err_cod = 0;
    if( dbhadr > 0 ) { //-- ������� ���������� �������
        int*    dbh = ( int* )dbhadr;
        int     err_cod = dbh[3];
        if( err_cod ==0 ) return 0;  //-- ������������ ������ ���� ������
    } else  //-- ������� ��� ������������� ���
        err_cod = -dbhadr;

        //-- ���������� ����������� ����� ���� ������
    char*   pc = err_buf;
    int     ost_len = buf_len;
    char*   pc_loc = 0;
    int     loc_cod = err_cod;
    int     tmp_len = 0;
    int     j = 0;
    int     errors[] ={BAD_PTR, BAD_NUMBER, BAD_TABLE_HANDLE, BAD_TABLE_NAME, BUFFER_SMALL,
                       DBN_NOT_EXIST, DBS_NOT_EXIST  };
    char    *err_mess[] = {"bad pointer", "bad number", "bad table handle; ", "bad table name; ", 
	"buffer is small; ", "dbn file not exist; ", "dbs file not exist; " };
    *pc = 0;    //-- ��������� ���������� ������ strcat

    if( loc_cod >= FATAL_ERROR ) {  //-- ���� ������
        pc_loc = "errors: ";  tmp_len = strlen(pc_loc);
        if( tmp_len  >= ost_len ) return 0;
        strcat( err_buf, pc_loc );  ost_len -= tmp_len;

            //-- ���������� ��� ��������� ������
        for( j = 0; j< sizeof(errors)/sizeof(int); j++ ) {
            if( loc_cod & errors[ j ] ){
                pc_loc = err_mess[ j ];  tmp_len = strlen(pc_loc);
                if( tmp_len + 20 >= ost_len ) return 0;
                strcat( err_buf, pc_loc );  ost_len -= tmp_len;
            }
        }

    }

        //-- ��������� �� ��������������
    int     warns[] = { REC_EMPTY, DBN_IS_EMPTY, OUT_OF_RANGE, REC_DELETED, BAD_SUBSTNUMBER };
    char*   warn_mes[] = { "record is empty; ", "table is empty; ",
                            "record number out of range; ","record deleted; ", "bad number of substring" };
    loc_cod = loc_cod &( FATAL_ERROR - 1 );
    if( loc_cod ) {  //-- ���� ��������������
        pc_loc = "wornings: ";  tmp_len = strlen(pc_loc);
        if( tmp_len + 20 >= ost_len ) return 0;
        strcat( err_buf, pc_loc );  ost_len -= tmp_len;
            //-- ���������� ��� ��������� ��������������
        for( j = 0; j< sizeof(warns)/sizeof(int); j++ ) {
            if( err_cod & warns[ j ] ){
                pc_loc = warn_mes[ j ];  tmp_len = strlen(pc_loc);
                if( tmp_len + 20 >= ost_len ) return 0;
                strcat( err_buf, pc_loc );  ost_len -= tmp_len;
            }
        }
    }

        //-- ������������ �����������
    tmp_len = strlen( comment );
    if( tmp_len + 20 >= ost_len ) return 0;
    strcat( err_buf, comment );  ost_len -= tmp_len;

        //-- ���������� ������ ������� ������
    return  ost_len;
};
//----------------------------------------------------------
//== 09 sen 2013 ==

//==  ��������  ������� ������ DBSN  =======================
int  getErrCod( int dbhadr ) {
    if( !dbhadr )  return BAD_TABLE_HANDLE;
    int*    dbh = (int*) dbhadr;
    return  dbh[3];
}
//----------------------------------------------------------
//==  ��������������� ������� ====

//== ������� �������� �������� ��������� ���� DBSN  ========
//--  ����������  ����� ������
//--  ��� -��� ������ ��� ��������� ������

//-- ���������� ������� (���� ��� ��������� ������)
int  getFragmOffsetIn( int*  dbh,  int &rec_offset ){
    int     rec_nom =   dbh[2];
    int     fhn =   dbh[0];
    //-- ��������� ������������� ������ ������
    fseek(  (FILE*)fhn,0,2);
    int     rec_count = ftell(  (FILE*) fhn ) /8;
    if( !rec_count )  {
        dbh[3] = DBN_IS_EMPTY;
        return  0;
    }
    //-- ��������� ���������� ��������� ���������
    fseek(  (FILE*)fhn, (  rec_nom-1)*8, 0);
    rec_offset = freadInt(  fhn );
    return   rec_nom;
}
//----------------------------------------------------------

//-- ������������� ������� ---
int  getFragmOffset( int  dbhadr,  int &rec_offset ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return getFragmOffsetIn( (int*)dbhadr,  rec_offset );
};
//----------------------------------------------------------

//----------------------------------------------------------

//== ������� ����� �������� ��������� ���� DBSN ===========
//--  ���������� ����� ������
//--  ��� -��� ������ ��� ��������� ������
//-- ���������� ������� (���� ��� ��������� ������)
int  getFragmSizeIn( int* dbh, int &rec_size ){
    int     rec_nom =   dbh[2];
    int     fhn =   dbh[0];
    //-- ��������� ������������� ������ ������
    fseek(  (FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*) fhn ) /8;
    if( !  rec_count)
    {	  dbh[3] = DBN_IS_EMPTY;
        return  0;
    }
    //-- ��������� ���������� ��������� ���������
    fseek(  (FILE*)fhn, (  rec_nom-1)*8 + 4, 0);
    rec_size = freadInt(   fhn );
    return   rec_nom;
}

//-- ������������� ������� ---
int  getFragmSize( int dbhadr, int &rec_size ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  getFragmSizeIn( (int*) dbhadr, rec_size );
};
//----------------------------------------------------------


//== ��������� ������� �� ������ ========================
//--  ���������� ����� ������� ������ ���� ��� �������
//--  ��� -��� ������ ��� ��������� ������

//-- ���������� ������� (���� ��� ��������� ������)
int isActiveIn( int* dbh ) {
    dbh[3] = 0; //-- ������� ��������������
    int     rec_nom =   dbh[2];
    int     fhn =   dbh[0];
    //-- ��������� ������������� ������ ������
    fseek(  (FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( ! rec_count)
    {	dbh[3] = DBN_IS_EMPTY;
        return  0;
    }
    //-- ��������� ���������� ��������� ���������
    fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
    int     rec_size = freadInt(   fhn );
    if(  rec_size < 0)
        return 0;
    return   rec_nom;
}

//-- ������������� ������� ---
int isActive( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  isActiveIn( (int*) dbhadr );
}
//----------------------------------------------------------

//== �������� ������� ������ ===============================
int  clearError( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*    dbh = (int*) dbhadr;
    dbh[3] = 0;
    return dbh[2];
}
//----------------------------------------------------------

//==  ��������� ����� ������� � ������� DBSN
//--  ���������� ����� ���������� �������
//-- (��������� � ������� ��������� ������)
//--  ��� ������������� ��� ��������� ������

//-- ���������� ������� (���� ��� ��������� ������)
int countFragmAIn( int* dbh ) {
    dbh[3] = 0; //-- ������� ��������������
    //-- ����������� ����������� ������
    int     fhn = dbh[0];
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( !rec_count )
        dbh[3] = DBN_IS_EMPTY;
    return  rec_count;
}

//-- ������������� ������� ---
int countFragmA( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  countFragmAIn( (int*) dbhadr );

};
//----------------------------------------------------------

//==  ����������������  ===
//==  �������� ����� �������� ���������  ===================
//--  ���������� ����� ������
//--  ��� ������������� ��� ��������� ������

//-- ���������� ������� (���� ��� ��������� ������)
int  getNomIn(int* dbh ){
    return  dbh[2];  //-- must be correct
}

//-- ������������� ������� ---
int  getNom(int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  getNomIn((int*) dbhadr );
}
//----------------------------------------------------------

//==  ���������� ����� �������� ���������  =================
//--  ���������� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������� ����� �� �������

//-- ���������� ������� (���� ��� ��������� ������)
int  setNomIn(int* dbh, int new_nom ) {
    dbh[3] = 0;  //-- ������� ��������������
    int     fr_count = countFragmAIn( dbh );
    if( fr_count == 0 ){
        dbh[3] |= OUT_OF_RANGE;  dbh[2] = 0; return dbh[2];
    };
    dbh[2] = new_nom;  //-- ������� ������
    if( new_nom < 1)  {
    	dbh[2] = 1;
        dbh[3] |= OUT_OF_RANGE;
        return  0;
    }
    if( new_nom > fr_count )  {
        dbh[2] = fr_count;
        dbh[3] |= OUT_OF_RANGE;
        return  0;
    }
    return  dbh[2];
}

//-- ������������� ������� ---
int  setNom(int dbhadr, int new_nom ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  setNomIn( (int *) dbhadr, new_nom );
}
//----------------------------------------------------------

//==  ��������� ����� �������� ���������  =================
//--  ���������� ����� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������� ����� �� �������( ��������� �������� �� �����)

//-- ���������� ������� (���� ��� ��������� ������)
int  incNomIn( int* dbh, int  count  )  {
    int new_nom = dbh[2] + count;
    return setNomIn( dbh, new_nom );
}

//-- ������������� ������� ---
int  incNom(int dbhadr, int  count  )  {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return   incNomIn( (int*) dbhadr, count  );
}
//----------------------------------------------------------


//==  ��������� ����� �������� ���������  =================
//--  ���������� ����� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������� ����� �� �������( ��������� �������� �� �����)

//-- ���������� ������� (���� ��� ��������� ������)
int decNomIn(int* dbh, int  count  )  {
    int new_nom = dbh[2] - count;
    return setNomIn( dbh, new_nom );
}

//-- ������������� ������� ---
int decNom(int dbhadr, int  count  )  {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return   decNomIn((int*) dbhadr, count );
}
//----------------------------------------------------------

//==  ���������� ������ ����� ���������  ===============
//--  ���������� ����� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������� ����� �� �������( ��������� �������� �� �����)

//-- ���������� ������� (���� ��� ��������� ������)
int  setFirstIn(int* dbh )  {
	return  setNomIn( dbh, 1);
}

//-- ������������� ������� ---
int  setFirst(int dbhadr )  {
	return  setNom(dbhadr, 1);
}
//----------------------------------------------------------

//==  ���������� ��������� ����� ���������  ============
//--  ���������� ����� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������� ����� �� �������( ��������� �������� �� �����)

//-- ���������� ������� (���� ��� ��������� ������)
int  setLastIn(int* dbh ){
    int new_nom = countFragmAIn( dbh );
    return  setNomIn( dbh, new_nom);
}

//-- ������������� ������� ---
int  setLast(int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  setLastIn( (int*) dbhadr );
}
//----------------------------------------------------------

//==  ������  ==============================================
//== ������� ������� �������� ���� DBSN  ===================
//--  ���������� ����� ������ ���
//--  ��� ������������� ��� ������
//--  ��� 0 ��� ������ ������

//-- ���������� ������� (���� ��� ��������� ������)
int  getFragmIn( int* dbh, char* fragm, int bufsize ) {
    int     rec_nom = dbh[2];
    if ( !rec_nom ){ //-- ��������� ������������� ������ ������
        dbh[3] |= DBN_IS_EMPTY;  return 0;
    }
        //-- ��������� ���������� ��������� ���������
    int     fhn = dbh[0];
    int     nom_off = (rec_nom-1)*8;
    fseek((FILE*)fhn, nom_off, 0);
    int     rec_offset = freadInt( fhn );
    int     rec_size = freadInt( fhn );
    if(rec_size == 0 )  { //--  ������ ������
    	*fragm = 0;  dbh[3] |= REC_EMPTY;   return  0;
    }
    if(rec_size < 0 )  {//--  ������ �������
    	rec_size = -rec_size;  dbh[3] |= REC_DELETED;
    }
    if( rec_size >= bufsize) { //-- �� ������� � �����
        dbh[3] |= BUFFER_SMALL;  return  -dbh[3];
    }
    //-- ��������� ��� ��������
    int     fhs = dbh[1];
    fseek((FILE*)fhs, rec_offset, 0);
    fread((void*) fragm, rec_size,1,(FILE*) fhs);
    fragm[rec_size]=0;
    if( dbh[3] ) return -dbh[3];
    return rec_nom;
}

//-- ������������� ������� ---
int  getFragm(  int dbhadr, char* fragm, int bufsize ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  getFragmIn( (int*) dbhadr, fragm, bufsize );
};
//----------------------------------------------------------

//== ������� �������� ���� DBSN � �������� �������
//-- ������ ������ ��� ����� �������������
//--  ���������� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������ ������

//-- ���������� ������� (���� ��� ��������� ������)
int  delFragmIn(  int* dbh ) {
	dbh[3] = 0; //-- ������� ��������������
		//-- ��������� ������������� ������ ������
	if(! countFragmAIn( dbh ))  return  0;
    int     rec_nom = dbh[2];
		//-- ������ ������������� ����� ��������� ���������
    int     fhn = dbh[0];
	fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
	int     rec_size = freadInt( fhn );
	if(rec_size > 0) {
	   	rec_size = -rec_size;
		fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
		fwriteInt( fhn, rec_size );
        dbh[3] |= REC_DELETED;
    }
	return rec_nom;
}

//-- ������������� ������� ---
int  delFragm(  int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  delFragmIn(  (int*) dbhadr );
};
//----------------------------------------------------------


//== ����� ������� �������� � ��������� ���� DBSN
//-- ������ ������ ��� ����� ���������������
//--  ���������� ����� ������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������ ������

//-- ���������� ������� (���� ��� ��������� ������)
int  undelFragmIn(  int* dbh )  {
	dbh[3] = 0; //-- ������� ��������������
		//-- ��������� ������������� ������ ������
	if(! countFragmAIn( dbh ))  return  0;
		//-- ������ ��������������� ����� ��������� ���������
    int     rec_nom = dbh[2];
    int     fhn = dbh[0];
	fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
	int     rec_size = freadInt( fhn );
	if(rec_size < 0)  {
		rec_size = -rec_size;
		fseek((FILE*)fhn, (rec_nom-1)*8+4, 0);
		fwriteInt( fhn, rec_size );
        dbh[3] &= ~REC_DELETED;
    }
	return rec_nom;
};


//-- ������������� ������� ---
int  undelFragm(  int dbhadr )  {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  undelFragmIn(  (int*) dbhadr );
};
//----------------------------------------------------------

//== ������ � ������� ����������  ==========================
//-- ��������� ������ �� 1 ��  countNblocks ������������

//==  ��������� ����� ������ ������� � ���� DBSN
//--  ���������� ����� ���������� ������ � ������ ���������� ��������
//--  ��� ������������� ��� ��������� ������
//--  ��� 0 ��� ������ �������
//--  block_size - ������ �����
//--  flag_sel - ������ ����� ����������
//--  ALL - ��� ������, ACTIVE - ������ ��������
//--  �� �������� ���������� ����� ������
int countNblocks( int dbhadr, int block_size, int flag_sel = ACTIVE_FRAGM ) {
        //-- ������������ ��������� ������
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);

    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- ������� ��������������
        //-- ������� ����� �������
    int     fhn = dbh[0];  //-- ����������� ����������� ������
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( !rec_count )  {   dbh[3] = DBN_IS_EMPTY;  return 0; }

    if(( flag_sel == ACTIVE_FRAGM)||
       ( flag_sel == ACTIVE_BACK))  {  //-- �������� ������ �������� ������
        int     active_count = 0,  rec_size;
        for( int rec_nom = 1; rec_nom <= rec_count; rec_nom++) {
                //-- ��������� ���������� ��������� ���������
            fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
            rec_size = freadInt(   fhn );
            if(  rec_size > 0) active_count++;
        }
        rec_count =  active_count;
        if( !rec_count )  return 0;
    }
//-- ������� ����� �������� ������
    int     block_count = rec_count / block_size;
    if( rec_count >  block_count *  block_size)
        block_count++;
    return  block_count;
};
//----------------------------------------------------------


//== �������� ����( ������ ������� ) ���������� �� ��� ������
//-- ���������� �������� ������ ����� <= block_size
//-- ��� ������������ ������ ����� ���������� 0
int getNblock( int dbhadr, int* pblock, int block_nom, int block_size, int flag_sel = ACTIVE_FRAGM ) {
        //-- ������������ ��������� ������ � ����� �� �������
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    if(  block_nom < 1 )  return 0;  //-- ������
    if( block_nom > countNblocks( dbhadr, block_size, flag_sel ) ) return 0;
        //-- ������� ����� �������
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- ������� ��������������
    int     fhn = dbh[0];  //-- ����������� ����������� ������
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int rec_nom,
        rec_size,
        active_nom = 0,
        count_nom = 0,
        beg_nom = 1;
        //-- ������������� ������ �������� �������
    switch( flag_sel ){
        case  ALL_FRAGM:  //-- ��������� ��� ������ ������
            rec_nom = ( block_nom - 1) * block_size + 1;
            while( block_size ) {
                *pblock++ = rec_nom++;
                block_size--;  count_nom++;
                if( rec_nom > rec_count)  break;
            }
            break;
        case  ACTIVE_FRAGM:  //-- ������ ������ ��������
                //-- ���� ������ �����
            beg_nom = ( block_nom - 1) * block_size + 1;
            for( rec_nom = 1; rec_nom <= rec_count; rec_nom++) {
                    //-- ��������� ���������� ��������� ���������
                fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                rec_size = freadInt(   fhn );
                if(  rec_size > 0) active_nom++;
                if(  active_nom == beg_nom )  break;
            }
                //-- ��������� ������ �������� ������� �����
            while( block_size )  {
                *pblock++ = rec_nom++;
                block_size--;  count_nom++;
                for( ; rec_nom <= rec_count; rec_nom++) {
                        //-- ��������� ���������� ��������� ���������
                    fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                    rec_size = freadInt(   fhn );
                    if(  rec_size > 0)  break;
                }
                if( rec_nom > rec_count)  break;
            }
            break;
        case  ALL_BACK:
            rec_nom = rec_count - ( block_nom - 1) * block_size;
            while( block_size ) {
                *pblock++ = rec_nom--;
                block_size--;  count_nom++;
                if( rec_nom < 1 )  break;
            }
            break;
        case  ACTIVE_BACK:
                //-- ���� ������ �����
            beg_nom = ( block_nom - 1) * block_size + 1;
            for( rec_nom = rec_count; rec_nom >= 1; rec_nom--) {
                    //-- ��������� ���������� ��������� ���������
                fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                rec_size = freadInt(   fhn );
                if(  rec_size > 0) active_nom++;
                if(  active_nom == beg_nom )  break;
            }
                //-- ��������� ������ �������� ������� �����
            while( block_size )  {
                *pblock++ = rec_nom--;
                block_size--;  count_nom++;
                for( ; rec_nom >= 1; rec_nom--) {
                        //-- ��������� ���������� ��������� ���������
                    fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
                    rec_size = freadInt(   fhn );
                    if(  rec_size > 0)  break;
                }
                if( rec_nom < 1)  break;
            }
            break;
    };
    return  count_nom;
};
//----------------------------------------------------------

//-- ����� ���������� ������ ����������  ����� �������� ----

//==  �������� �������� ������ � ���� ���������� DBSN
//--  ���������� ����� ���������� ������(������ �� ����������)
//--  ��� -��� ��������� ������

//-- ���������� ������� (���� ��� ��������� ������)
int  addBinFragmIn( int* dbh, char* fragm, int len_fragm) {
    dbh[3] = 0; //-- ������� ��������������
        //-- ����������� ����������� ������
    int     fhn = dbh[0];
    int     fhs = dbh[1];
        //-- ������������ ��������
    fseek((FILE*)fhs,0,2);
    int     rec_offset = ftell( (FILE*)fhs );
    fwrite( (void*)fragm, len_fragm, 1, (FILE*)fhs );
        //-- ������������ �������� ����������
    fseek((FILE*)fhn,0,2);
    fwriteInt( fhn, rec_offset);
    fwriteInt( fhn, len_fragm);
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    dbh[2] = rec_count;
    return dbh[2];
};

//-- ������������� ������� ---
int  addBinFragm( int dbhadr, char* fragm, int len_fragm ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  addBinFragmIn( (int*) dbhadr, fragm, len_fragm );
};
//----------------------------------------------------------

//==  �������� ��������� ������ � ���� ���������� DBSN
//--  ���������� ����� ���������� ������(������ �� ����������)
//--  ��� -��� ��������� ������
int  addFragm( int dbhadr, char* fragm ) {
    int     len_fragm = getLen( fragm );
    return  addBinFragm( dbhadr, fragm, len_fragm );
};
//----------------------------------------------------------

//==  �������� �������� �������� � ������� ������ ���� DBSN
//--  ���������� ����� ������
//--  ��� -��� ��������� ������
//--  ��� 0 ��� ������ ��� ������

int  setBinFragm(  int dbhadr, char* fragm, int len_fragm ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int* dbh = (int*) dbhadr;
    int     rec_nom = dbh[2];
        //-- ��������� ������������� ������ ������
    int     fr_count = countFragmAIn( dbh );
    if((! fr_count )||(rec_nom < 1)||(rec_nom > fr_count)) {
        dbh[3] |= OUT_OF_RANGE;
        return  0;
    }
        //-- ������������ ���������� ��������
    int     fhn = dbh[0];
    int     fhs = dbh[1];
    fseek((FILE*)fhs,0,2);
    int     rec_offset = ftell( (FILE*)fhs );
    fwrite((void*)fragm, len_fragm, 1, (FILE*)fhs);
        //-- �������������� �������� ����������
    fseek((FILE*)fhn, (rec_nom-1)*8, 0);
    fwriteInt( fhn, rec_offset);
    fwriteInt( fhn, len_fragm);
    return rec_nom;
}
//----------------------------------------------------------


//==  �������� ����� � ������� ������ ���� DBSN
//--  ���������� ����� ������
//--  ��� -��� ��������� ������
//--  ��� 0 ��� ������ ��� ������
int  setFragm(  int dbhadr, char* fragm ) {
	int     len_fragm = getLen( fragm );
    return  setBinFragm(  dbhadr, fragm, len_fragm );
};
//----------------------------------------------------------

//== active functions  ==

//==  ������� � ������� ��������� ���������
//--    ���������� ����� ���������
//--    ��� -��� ��������� ������
//--    ��� 0 ��� ������������� ( ��� �������� ������� )
int    gotoFirst( int dbhadr ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- ������� ��������������

    int     fhn = dbh[0];  //-- ����������� ����������� ������
        //-- ������� ����� �������
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
        //--  ���� ������ �������� �������
    int rec_nom,
        rec_size;
    for( rec_nom = 1; rec_nom <= rec_count; rec_nom++)  {
            //-- ��������� ���������� ��������� ���������
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            return  rec_nom;
        };
    }
    return  0;  //-- �� �����
};
//----------------------------------------------------------

//==  ������� ������ ����� ��������� �������� ����������
//--    ���������� ����� ���������
//--    ��� -��� ��������� ������
//--    ��� 0 ��� ������������� ( ��� ������� �������� ������� )
int    goForward( int dbhadr, int step_count ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- ������� ��������������

    int     fhn = dbh[0];  //-- ����������� ����������� ������
        //-- ������� ����� �������
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int rec_nom,
        rec_size;

        //--  �������� �������� ���������
    for( rec_nom = dbh[2]; rec_nom <= rec_count; rec_nom++)  {
            //-- ��������� ���������� ��������� ���������
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            if( step_count >0) {
                step_count--;  continue;
            }
            return  rec_nom;
        };
    }
    return  0;
};
//----------------------------------------------------------

//==  ������� ����� ����� ��������� �������� ����������
//--    ���������� ����� ���������
//--    ��� -��� ��������� ������
//--    ��� 0 ��� ������������� ( ��� ������� �������� ������� )
int    goBack( int dbhadr, int step_count ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- ������� ��������������

    int     fhn = dbh[0];  //-- ����������� ����������� ������
    int rec_nom,
        rec_size;
        //--  �������� �������� ���������
    for( rec_nom = dbh[2]; rec_nom >= 1; rec_nom--)  {
            //-- ��������� ���������� ��������� ���������
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            if( step_count > 0) {
                step_count--;  continue;
            }
            return  rec_nom;
        };
    }
    return  0;
};
//----------------------------------------------------------

//==  ������� � ���������� ��������� ���������
//--    ���������� ����� ���������
//--    ��� -��� ��������� ������
//--    ��� 0 ��� ������������� ( ��� �������� ������� )
int    gotoLast( int dbhadr ){
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int*  dbh = (int*) dbhadr;
    dbh[3] = 0; //-- ������� ��������������

    int     fhn = dbh[0];  //-- ����������� ����������� ������
        //-- ������� ����� �������
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    int     rec_nom,
            rec_size;

        //--  ���� ��������� �������� �������
    for( rec_nom = rec_count; rec_nom >= 1; rec_nom--)  {
            //-- ��������� ���������� ��������� ���������
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) {
            dbh[2] = rec_nom;
            return  rec_nom;
        };
    }
    return  0;
};
//----------------------------------------------------------


//== 28 sen 2013 ===========================================
//-- ��� ������� ������ � �����������

//== �������� ��������� � ������ �����
//-- ��������� �������� � ����� pcres
//-- ���������� ����� ���������� ��� ����� ��� ������
int  addSubst(char* pc, char* pcres, int res_size, char* pcsubst) {
        //-- ��������� ��������
    if( !pc || !pcres || !pcsubst ) return - BAD_PTR;
    int     st_len = getLen( pc),
            subst_len = getLen( pcsubst );
    if( res_size < st_len + subst_len + 2 ) return - BUFFER_SMALL;
       //-- ���������� ������
    char    *pc_src = pc, *pc_dst = pcres;
    while( *pc_src )
        *pc_dst++ = *pc_src++;
        //-- ���������� ����� ��������� � ��������� �� ������
    if( pc_dst != pcres )
        *pc_dst++ = '|';
    while( *pcsubst )
        *pc_dst++ = *pcsubst++;
    *pc_dst = 0;
        //-- ���������� ����� ��������� ������
    return pc_dst - pcres - 1;
};
//----------------------------------------------------------

//-- �������� ����� ��������� � ������ ����� ������
//-- ��������� ��������� � ����� pcres
//-- ���������� ����� ���������� ��� ����� ��� ������
int insSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst) {
        //-- ��������� ��������
    if( !pc || !pcres || !pcsubst ) return - BAD_PTR;
    int     st_len = getLen( pc),
            subst_len = getLen( pcsubst );
    if( res_size < st_len + subst_len + 2 ) return - BUFFER_SMALL;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
        //-- �������� � ����� �������� �� ������ ���������
    char    *pc_src = pc, *pc_dst = pcres;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
    *pcres = 0;
    while( nom_sub ){ //-- �������� �� ����������� ������������
        if( *pc_src =='|')
            nom_sub--;
        *pc_dst++ = *pc_src++;
    }
        //-- �������� ����� ��������� � ������������
    while( *pcsubst ){
        *pc_dst++ = *pcsubst++;
    }
    *pc_dst++ = '|';
        //-- �������� ������� �������� ������
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pcres - 1; //--���������� ���������
};
//----------------------------------------------------------

//-- ��������� ����� �������� ���������
//-- ���������� ����� ��������� ��� ����� ��� ������
int substLen(char* pc, int nom_sub ) {
        //-- ��������� ��������
    if( !pc  ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
        //-- ���� ������ ���������
    char    *pc_src = pc;
    while( nom_sub ){ //-- ���������� ������ �� ����������� ������������
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ���� ����� ���������
    char*  pc_beg = pc_src;
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
        //-- ���������� �� �����
    return  pc_src - pc_beg;
}
//----------------------------------------------------------

//-- �������� ��������� �� ����� �������� � ������ ����� ������
//-- ��������� ��������� � ����� pcres
//-- ���������� ����� ���������� ��� ����� ��� ������
int setSubst(char* pc, char* pcres, int res_size, int nom_sub, char* pcsubst) {
        //-- ��������� ��������
    if( !pc || !pcres || !pcsubst ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc),
            oldsubst_len = substLen( pc, nom_sub ),
            newsubst_len = getLen( pcsubst );
    if( res_size < st_len + newsubst_len - oldsubst_len + 2 ) return - BUFFER_SMALL;
        //-- �������� � ����� �������� �� ������ ���������
    char    *pc_src = pc, *pc_dst = pcres;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
    *pcres = 0;
    while( nom_sub ){ //-- �������� �� ����������� ������������
        if( *pc_src =='|')
            nom_sub--;
        *pc_dst++ = *pc_src++;
    }
        //-- �������� ����� ��������� � ������������
    while( *pcsubst ){
        *pc_dst++ = *pcsubst++;
    }
    //*pc_dst++ = '|';
        //-- ���������� ������ ���������
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
        //-- �������� ������� �������� ������
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pcres - 1; //--���������� ���������
};

//----------------------------------------------------------

//-- �������� ��������� (�������� �� ������ �������� )
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int clearSubst(char* pc, int nom_sub ) {
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
        //-- ������� ������ ������ ���������
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ���������� ���������� ��������� �� �����������
    char*    pc_dst = pc_src;
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
        //-- �������� ������� �������� ������
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--���������� ���������
};
//----------------------------------------------------------

//-- ��������� ������� ���������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int cutSubst(char* pc, int nom_sub ) {
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
        //-- ������� ������ ������ ���������
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ���������� ���������� ��������� �� �����������
    char*    pc_dst = pc_src;
    while( (*pc_src != '|') &&( *pc_src != 0))
        pc_src++;
    if( *pc_src == '|')
        pc_src++;  //-- ������� �����������
        //-- �������� ������� �������� ������
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--���������� ���������
};
//----------------------------------------------------------

//-- ��������� ������� ������ ��������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int cutSubstGroup(char* pc, int nom_sub, int cut_count = 1 ) {
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
        //-- ������� ������ ������ ���������
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ��������� �� ���������� ����� ��������
    if( nom_sub + cut_count >= subst_count){
        if( nom_sub > 0)   pc_src--;
        *pc_src = 0;
        return pc_src - pc;
    }
        //-- ���������� ���������� ��������� �� ������� �����������
    char*    pc_dst = pc_src;
    while( cut_count ){
        if( *pc_src == '|')   cut_count--;
        pc_src++;
    }
        //-- �������� ������� �������� ������
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--���������� ���������
};
//----------------------------------------------------------
//-- ������� ��� ��������� ����� �������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int cutLeft(char* pc, int nom_sub ) {
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
        //-- ������� ������ ������ ���������
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ���������� ���������� ��������� �� �����������
    char*    pc_dst = pc;
        //-- �������� ������� �������� ������
    while( *pc_src ){
        *pc_dst++ = *pc_src++;
    }
    *pc_dst = 0;
    return pc_dst - pc - 1; //--���������� ���������
};

//----------------------------------------------------------

//-- ������� ��� ��������� ������� � �������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int cutRight(char* pc, int nom_sub ) {
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n')  pc[st_len-1]=0; //-- ������������� ��������� ����� ������
    if( nom_sub == 0 ){ //-- ����������� ������ - ������� ��� ������
        *pc = 0;
        return  0;
    }
        //-- ������� ������ ������ ���������
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- ��������� ����� ������
    pc_src--;
    *pc_src = 0;
    return pc_src - pc - 1;  //--���������� ���������
};
//----------------------------------------------------------

//-- ���������� ��������� � ����������
//-- �������� ����������� �� ������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int uniteSubst(char* pc, int nom_sub ) {
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))
        return  - BAD_NUMBER;
    int     st_len = getLen( pc);
    char    *pc_src = pc;
    if(pc[st_len-1]=='\n'){
         pc[st_len-1]=0; //-- ������������� ��������� ����� ������
         st_len--;
    }
    if( !nom_sub ){  //-- ��� ������� ��������� ��� ����������
        return  st_len;
    }
        //-- ������� ������ ������ ���������
    while( nom_sub ){
        if( *pc_src =='|')
            nom_sub--;
        pc_src++;
    }
        //-- �������� ����������� �� ������
    pc_src--;  *pc_src = ' ';
    return  st_len;  //--���������� ���������
};
//----------------------------------------------------------

//-- 27 jan 2014 ---
//-- ���������� �������� �������� �������(�������)

//== ���������� ����� ����� � �������� ����� ������ �������� ���������
//-- ���������� ����� ��������� ��� ����� ������
//== ���������� ������� ��� �������� �� ������
int 
setBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long src ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
    int*    pl = (int*)(pcFragm + offset);
    *pl = src;
    return  fragmLen;
}
//== ������� �������
int 
setBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long src ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  setBinnumIn( pcFragm, fragmLen, nom_sub, nom_num, src );
}
//----------------------------------------------------------

//== �������� ����� � �������� ������� �� ������ �������� ���������
//-- ���������� ����� ��������� ��� ����� ������
//-- � �� ������ dst - ����� ���� �� ���� ������
int 
getBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long &dst ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
    dst = *(int*)(pcFragm + offset);
    return  fragmLen;
}
//== ������� �������
int 
getBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num, long &dst ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  getBinnumIn( pcFragm, fragmLen, nom_sub, nom_num, dst );
}
//----------------------------------------------------------


//== ������� ����� � �������� ������� �� ������ �������� ���������
//-- ���������� ����� ��������� ��� ����� ������
//-- ���������� �������
int 
cutBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
        //--�������� ����� �� 4 ����� ������ ����� ���������
    for( int  j = offset; j < fragmLen - 4; j++) {
        pcFragm[j] = pcFragm[j+4];
    }
        //-- ��������� ����� ��������� � ���������
    offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int*    pl = (int*) (pcFragm + offset );
    *pl -= 4;
    fragmLen -= 4;
    return  fragmLen;
}
//== ������� ������� ==
int 
cutBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  cutBinnumIn( pcFragm, fragmLen, nom_sub, nom_num );
}
//----------------------------------------------------------

//== �������� ����� ����� � ����� ������ �������� ���������
//-- ���������� ����� ��������� ��� ����� ������
//== ���������� ������� ��� �������� �� ������
int 
addBinnumIn(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long src ){
        //-- �������� � ����� ������ ����� ������
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub );
    int     loc_nom = 0;
    for( loc_nom = 0; loc_nom < offset; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- ��������� ����� �����
    int*    pl = (int*)(pcFragm + offset);
    *pl = src;
        //-- ���������� ����� ���������
    pcBuf += 4;
    for( loc_nom = offset; loc_nom < fragmLen; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- �������� ����� �������� � ���������
    offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    pl = (int*) (pcFragm + offset );
    *pl += 4;
    fragmLen += 4;
    return  fragmLen;
}
//== ������� �������
int 
addBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long src ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( lenBuf < fragmLen + 4 ) return  -BUFFER_SMALL;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    return  addBinnumIn( pcFragm, fragmLen, pcBuf, lenBuf, nom_sub, src );
}
//----------------------------------------------------------

//== �������� ����� ����� � �������� ����� ������ �������� ���������
//-- ���������� ����� ��������� ��� ����� ������
//== ���������� ������� ��� �������� �� ������
int 
insBinnumIn(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long nom_num, long src ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub) + nom_num*4;
        //-- �������� � ����� ������ ����� ������
    int     loc_nom = 0;
    for( loc_nom = 0; loc_nom < offset; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- ��������� ����� �����
    int*    pl = (int*)(pcFragm + offset);
    *pl = src;
        //-- ���������� ����� ���������
    pcBuf += 4;
    for( loc_nom = offset; loc_nom < fragmLen; loc_nom++ ){
        *pcBuf++ = *pcFragm++;
    }
        //-- �������� ����� �������� � ���������
    offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    pl = (int*) (pcFragm + offset );
    *pl += 4;
    fragmLen += 4;
    return  fragmLen;
}
//== ������� �������
int 
insBinnum(char* pcFragm, int fragmLen, char* pcBuf, int lenBuf, long nom_sub, long nom_num, long src ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( lenBuf < fragmLen + 4 ) return  -BUFFER_SMALL;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num <= 0 )  return -BAD_NUMBER;
    return  insBinnumIn( pcFragm, fragmLen, pcBuf, lenBuf, nom_sub, nom_num, src );
}
//----------------------------------------------------------


//== �������� ������� 2 ����� � ������ �������� ���������
//-- ���������� ����� ��������� ��� ����� ������
//== ���������� ������� ��� �������� �� ������
int 
xchgBinnumIn(char* pcFragm, int fragmLen, long nom_sub, long nom_num1, long nom_num2 ){
    int     offset = binsubstOffsetIn( pcFragm, fragmLen, nom_sub);
    int*    pl = (int*)(pcFragm + offset);
    int*    pl1 = pl+nom_num1;
    int*    pl2 = pl+nom_num2;
    int     temp = *pl1;  *pl1 = *pl2;  *pl2 = temp;
    return  fragmLen;
}
//== ������� �������
int 
xchgBinnum(char* pcFragm, int fragmLen, long nom_sub, long nom_num1, long nom_num2 ){
    if (( !pcFragm )||( !fragmLen))  return - BAD_PTR;
    if( nom_sub > binsubstCountIn( pcFragm, fragmLen) )  return -BAD_NUMBER;
    if( nom_num1 >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num1 <= 0 )  return -BAD_NUMBER;
    if( nom_num2 >= binsubstSizeIn( pcFragm, fragmLen, nom_sub ))  return -BAD_NUMBER;
    if( nom_num2 <= 0 )  return -BAD_NUMBER;
    return  xchgBinnumIn( pcFragm, fragmLen, nom_sub, nom_num1, nom_num2 );
}
//----------------------------------------------------------
//== 31 jan 2014 ====
//== ������������ �������  �� ���������� ����� � �����-�����������
//-- ��������, ��� ��������� ����� �� ����� �� ����� "�����"
//-- ��� ��� ��������� ����� �� �������, ��� ������ �� ����������
//-- ��������� ���������� ���������� ��� ������������� ��� ������
//-- ������ add_key ����������, ���� �� �������� ���� �������� �����
//-- � ������ ���������, ��������� ��������:
//       0 - ����������� �� ����� �������� ����������
//       1 - ����������� � ������ ����������� ���������
//      -1 - ����������� � ������ ���������� ���������
int 
formDBSN( char* txtFile, char* keyWord, int add_key ){
    int     buf_size = 4096;  //--������ ��������� ������
    char    buf[9182];        //-- ��� ����� ��� ���������� �������� ������
    char    fil_dbs[128];    //--��� DBS �����
    char    fil_dbn[128];    //--��� DBN �����
        //-- ��������� ��� DBS � DBN �����
    char*    pc_src = txtFile;
    char*    pc_dbs = fil_dbs;
    char*    pc_dbn = fil_dbn;
    while( (*pc_src)&&(*pc_src !='.')) {
        *pc_dbs++ = *pc_src;  *pc_dbn++ = *pc_src; pc_src++;
    }
    for( pc_src =".db"; *pc_src; pc_src++) {
        *pc_dbs++ = *pc_src; *pc_dbn++ = *pc_src;
    }
    *pc_dbs++ ='s'; *pc_dbs = 0; *pc_dbn++ ='n'; *pc_dbn = 0;

    FILE*   fsrc = fopen( txtFile, "rb");
    if(!fsrc) {               //-- �� ������ ������� �������� ����
        return -DBS_NOT_EXIST;
    }
        //-- ��������� ������ ( ������� ) ������ ������
    int     data_len = fread(buf, 1, 2*buf_size, fsrc );  //-- ������ � ������
    if( !data_len )  {
        fclose( fsrc );  return -DBS_NOT_EXIST;
    }
        //-- ��������� DBS, DBN �����
    FILE*     filhs =  fopen(fil_dbs, "wb");
    if(!filhs) {               //-- �� ������ ������� ���� dbs
        return -DBS_NOT_EXIST;
    }
    int     fhn = (int)  fopen(fil_dbn, "wb");
    if(!fhn){               //-- �� ������ ������� ���� dbn
        return -DBN_NOT_EXIST;
    }
        //-- �������� ��������� - ����������
    int     fragm_count = 0;
    int     keyword_len = strlen( keyWord );
    int     rec_offset = 0;
    int     fragm_len = 0;
    int     buf_offset = 0;  //-- ������� ���������� ��������� ����� � ������
    int     loc_nom;
        //-- ���� �� �������� �����
    while( data_len == 2*buf_size){
            //-- ���������� ����� � dbs ����
        fwrite(buf, 1, buf_size, filhs );
            //-- ������������� ������ �������� ������
        for( loc_nom = 0; loc_nom < buf_size; loc_nom++ ){
            if( ! strncmp( buf+loc_nom, keyWord, keyword_len ) ) {
                    //-- ����� �������� ����� - ���������� � ����������� �� �����
                fwriteInt( fhn, rec_offset );
                switch( add_key ){
                    case   1:  //-- �������� �������� ����� � ����� ��������
                        fragm_len += loc_nom - buf_offset + keyword_len;
                        rec_offset += fragm_len ;
                        buf_offset = loc_nom + keyword_len;
                        break;
                    case  -1:  //-- �������� �������� ����� � ������ ����������
                        fragm_len += loc_nom - buf_offset;
                        rec_offset += fragm_len;
                        buf_offset = loc_nom;
                        break;
                    default:   //-- �� ��������� ���� ����� � ���������
                        fragm_len += loc_nom - buf_offset;
                        rec_offset += fragm_len + keyword_len;
                        buf_offset = loc_nom + keyword_len;
                }
                fwriteInt( fhn, fragm_len );
                fragm_len = 0;
                fragm_count++;
            }
        }
            //-- �������� ������ �������� ������ � ������ � �������� ������ ��
        for( loc_nom = 0; loc_nom < buf_size; loc_nom++)
            buf[ loc_nom ] = buf[ loc_nom + buf_size ];
        fragm_len += buf_size - buf_offset;
        buf_offset = 0;
            //-- ���������� ���� �� ������ ����� ������
        data_len = buf_size + fread(buf + buf_size, 1, buf_size, fsrc );
    }
       //-- ���������� ������� ������ � dbs ����
    fwrite(buf, 1, data_len, filhs );
       //-- ��������� ��������� ����� �����
    for( loc_nom = 0; loc_nom < data_len - keyword_len; loc_nom++ ){
        if( ! strncmp( buf+loc_nom, keyWord, keyword_len ) ) {
                //-- ����� �������� ����� - ����������
            fragm_len += loc_nom - buf_offset;
            fwriteInt( fhn, rec_offset );
            fwriteInt( fhn, fragm_len );
            rec_offset += fragm_len;
            fragm_len = 0;
            buf_offset = loc_nom;
            fragm_count++;
        }
    }
        //-- ��������� ����� ����� ( �� ��������� �������� ������ )
    fragm_len += data_len - buf_offset;
    fwriteInt( fhn, rec_offset );
    fwriteInt( fhn, fragm_len );
    fragm_count++;
        //-- ���������� ������
    fclose( fsrc );
    fclose( filhs);
    fflush( filhs );
    fclose((FILE*)fhn);
    fflush( (FILE*)fhn );
    return  fragm_count;
}
//----------------------------------------------------------

//== 09 sen 2013 ===========================================
//-- work with ref -----------------------------------------

//== ������� ��������� ���� �� ������� ���������� �� ����� �����
//-- ���������� ��� ������ 0
//-- ��� ������������� ��� ������
int createDBRF( char*  filref_name ) {
        //-- ��������� ��� ������
    char*  fil_dbr = new char[ getLen( filref_name )+ 5];
    filref_name = trim(filref_name, fil_dbr);
    strcat( fil_dbr, ".dbr");
    int     fhr = (int)fopen(fil_dbr, "w");
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- �� ������ ������� ���� dbr
    fclose((FILE*) fhr );
    return  0;
}
//----------------------------------------------------------

//== ������� ��������� ���� �� ������� ���������� DBSN
//--  ���������� ���������� ����� ��� ������
//--  ��� ������������� ��� ������
int openDBRF( char* filref_name ) {
        //-- ��������� ��� ������
    char*  fil_dbr = new char[ getLen( filref_name )+ 5];
    filref_name = trim(filref_name, fil_dbr);
    strcat( fil_dbr, ".dbr");
    int     fhr = (int)fopen(fil_dbr, "rb+");
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- �� ������ ������� ���� dbr
    return  fhr;
}
//----------------------------------------------------------

//== ������� ��������� ���� �� ������� ���������� DBSN
//--  ���������� 0 ��� ������
//--  ��� ������������� ��� ������
int closeDBRF( int fhr ) {
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- �� ������ ������� ���� dbr
    fclose((FILE*)fhr);
    return  0;
}
//----------------------------------------------------------

//== ���������� ���������� ������� � ����� ������
//--  ���������� ��� ��������� ������
int countRefIn( int fhr ) {
    fseek((FILE*)fhr,0,2);
    return ftell( (FILE*)fhr ) /4;
};

//-- ������� � ���������� ������
int countRef( int fhr ) {
    if( !fhr ) return  - DBR_NOT_EXIST;  //-- ��� ����� dbr
    return  countRefIn( fhr );
};
//----------------------------------------------------------

//== �������� ����� ��������� �� ������ �� ����
//--  ���������� ����� ��������� ��� ������
//--  ��� ������������� ��� ������
//--  ���������� ��� ��������� ������
int  getRefIn(  int fhr, int ref_nom ){
    fseek((FILE*)fhr,(ref_nom-1)*4,0);
    return   freadInt( fhr );  //-- �������� ����� ���������
}

//-- ������� � ���������� ������
int  getRef(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    if(( ref_nom < 1)||(ref_nom > ref_count)) return -OUT_OF_RANGE;
    int     fragm_nom = getRefIn(  fhr, ref_nom );  //-- �������� ����� ���������
    if( fragm_nom < 0 )  fragm_nom *= -1;
    return  fragm_nom;
};
//----------------------------------------------------------

//== �������� �������� �� ������ �� ����
//--  ���������� ����� ������ ��� ������
//--  ��� ������������� ��� ������
int  getFragmByRef(  int dbhadr, int fhr, int ref_nom,  char* fragm, int bufsize ){
    int     fragm_nom = getRef(  fhr, ref_nom );
    if( fragm_nom <= 0 ) return fragm_nom;
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    setNom( dbhadr, fragm_nom );
    fragm_nom =  getFragm( dbhadr, fragm, bufsize );
    if( fragm_nom < 0 ) return fragm_nom;
    return ref_nom;
};
//----------------------------------------------------------


//== �������� � ��������� ���� ����� ���������
//--  ���������� ����� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���������� ��� ��������� ������
int  addRefIn(  int fhr, int fragm_nom ){
    fseek((FILE*)fhr,0,2);
    fwriteInt( fhr, fragm_nom );
    fseek((FILE*)fhr,0,2);
    return ftell( (FILE*)fhr ) /4;
}

//-- ������� � ���������� ������
int  addRef(  int fhr, int fragm_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    return  addRefIn( fhr, fragm_nom );
}
//----------------------------------------------------------

//== �������� � ��������� ����� ����� ��������� � ������ �������
//--  ���������� ����� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���������� ������� --
int  setRefIn(  int fhr, int ref_nom, int fragm_nom ){
    fseek((FILE*)fhr,(ref_nom-1)*4,0);
    fwriteInt( fhr, fragm_nom );
    return  ref_nom;
}

//-- ������� � ���������� ������
int  setRef(  int fhr, int ref_nom, int fragm_nom ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr ))) return -OUT_OF_RANGE;
        //-- ���������� � ������ ����� ����� ��������
    return  setRefIn(  fhr, ref_nom, fragm_nom );
}
//----------------------------------------------------------

//== ������� � ��������� ����� ����� ��������� � ������ �������
//--  ���������� ����� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���������� ������� --
int  delRefIn(  int fhr, int ref_nom ){
    int     fragm_nom = getRefIn( fhr, ref_nom );  
    if( fragm_nom > 0 ) //-- ���������� � ������ ����� ����� ��������
        setRefIn(fhr, ref_nom, -fragm_nom);
    return  ref_nom;
}

//-- ������� � ���������� ������
int  delRef(  int fhr, int ref_nom ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr ))) return -OUT_OF_RANGE;
    return  delRefIn(  fhr, ref_nom );
}
//----------------------------------------------------------

//== ����� �������� � ��������� �����
//--  ���������� ����� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���������� ������� --
int  undelRefIn(  int fhr, int ref_nom ){
    int     fragm_nom = getRefIn( fhr, ref_nom );
    if( fragm_nom < 0 ) //-- ���������� � ������ ����� ����� ��������
        setRefIn(fhr, ref_nom, -fragm_nom);
    return  ref_nom;
}

//-- ������� � ���������� ������
int  undelRef(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr ))) return -OUT_OF_RANGE;
    return  delRefIn(  fhr, ref_nom );
}
//----------------------------------------------------------

//== �������� ������� ������ �� ���������
//--  ���������� ����� ������ ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���������� ������� --
int  xchgRefIn( int  fhr, int  ref_nom1,  int ref_nom2 ) {
        //-- ������ ������ ����������
    int     fragm_nom1 = getRefIn( fhr, ref_nom1 );
    int     fragm_nom2 = getRefIn( fhr, ref_nom2 );
        //-- ���������� �� � �������������
    setRefIn( fhr, ref_nom1, fragm_nom2);
    setRefIn( fhr, ref_nom2, fragm_nom1);
    return  ref_nom1;
};

//-- ������� � ���������� ������
int  xchgRef( int  fhr, int  ref_nom1,  int ref_nom2 ) {
        //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr);
    if(( ref_nom1 < 1)||(ref_nom1 > ref_count)) return -OUT_OF_RANGE;
    if(( ref_nom2 < 1)||(ref_nom2 > ref_count)) return -OUT_OF_RANGE;
    return  xchgRefIn( fhr, ref_nom1, ref_nom2 );
};
//----------------------------------------------------------

//==  �������� ��������� � ��������� ����� ������ � ������ �������
//--  ���������� ����� ���������� ��� ��������� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���� �� �������������� ��������������
//--  ���������� ������� --
int  cutRefIn(  int fhr, int ref_nom ){
        //-- �������� ��� ���������� ����� � ������
    int     frnom_loc,
            refnom_loc,
            ref_count = countRefIn( fhr);
    for( refnom_loc = ref_nom; refnom_loc < ref_count; refnom_loc++) {
        frnom_loc = getRefIn( fhr, refnom_loc+1);
        setRefIn( fhr, refnom_loc, frnom_loc);
    }
        //-- ��������� ������ �����
    ftruncate(fileno( (FILE*)fhr ), ref_count*4 - 4);
    if( ref_nom < ref_count )
        return  ref_nom;
    return  ref_count - 1;
}


//-- ������� � ���������� ������
int  cutRef(  int fhr, int ref_nom ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn(fhr))) return -OUT_OF_RANGE;
    return  cutRefIn(fhr, ref_nom );
}
//----------------------------------------------------------

//== �������� � ��������� ����� ����� ��������� � ������ �������
//--  ���������� ����� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���� �� �������������� ��������������
//--  ���������� ������� --
int  insRefIn(  int fhr, int ref_nom, int fragm_nom ){
        //-- �������� ������� � ������� ����� �����
    int     frnom_loc,
            refnom_loc,
            ref_count = countRefIn( fhr);
    for( refnom_loc = ref_count; refnom_loc >= ref_nom; refnom_loc--) {
        frnom_loc = getRefIn( fhr, refnom_loc);
        setRefIn( fhr, refnom_loc, frnom_loc + 1);
    }
        //-- ���������� � ������ ����� ����� ��������
    return  setRefIn( fhr, ref_nom, fragm_nom);
}

//-- ������� � ���������� ������
int  insRef(  int fhr, int ref_nom, int fragm_nom ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(ref_nom > countRefIn( fhr))) return -OUT_OF_RANGE;
    return  insRefIn( fhr, ref_nom, fragm_nom);
}
//----------------------------------------------------------


//== 02 mar 2014 ===========================================
//-- filters and sorting -----------------------------------


//== �������� ������ �� ��������� �����
//--  ������ ������ ��� ������ ����������  � ��������� ����� ��������������
//--  ���������� ����� ������� � ��������� �����
//--  ��� ������������� ��� ������
int   clearFiltr(  int fhr ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        undelRefIn( fhr, ref_nom );
    };
    return  ref_count;
};
//----------------------------------------------------------

//== �������� �������� ��������� � �������� �������
//-- ���������� �������� ��� ������������� ��� ������
//-- ������������� ����� ��������� ������� �� ����� ������
int getSubstOffset(char* pc, int nom_sub) {
    if( !pc ) return - BAD_PTR;
    int     subst_count = substCount(pc);
    if( nom_sub <0) nom_sub += subst_count; //-- ����� ������� � �����
    if( (nom_sub <0)||(nom_sub >= subst_count))   return  - BAD_NUMBER;
    char    *pc_tmp = pc;
    while( nom_sub ){
        if( *pc_tmp == '|' ) nom_sub--;
        pc_tmp++;
    }
    return  pc_tmp - pc;
};

//== �������� ������ �� ��������� ����
//--  �������� �� ����� ���������� �����
//--  ��� ������������� ������ ��������� �������
//--  � ���� ��� �� ����������� ������ ������ �������������
//--  ���������� ����� �������� ������ � ��������� �����
//--  ��� ������������� ��� ������
int  addFiltr(  int dbhadr, int fhr, int subst_nom, char* condition, char* value ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    int     fragm_nom = 0;
    int     flag_cond = 0;   //-- ������� ���������� �������
    int     active_count = 0;  //-- ����� �������� ������
    int     res = 0;
    int     buf_size = 4096;
    char*   fragm_buf = new char[buf_size];  //-- ����� ��� ���������
    int     fragm_size = 0;
    int     subst_count = 0;
    int     subst_offset = 0;
    int     subst_len = 0;
    int     isub = 0,   //-- ����� �������� ���������
            ival = 0;   //-- ����� �������� ���������
    char*   pc_subst = 0;
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fragm_nom = getRefIn( fhr, ref_nom );
        if( fragm_nom <= 0 ) continue;  //-- ���������� ��������� ������
            //-- �������� ������ ���������
        flag_cond = 0;
        res = setNom( dbhadr, fragm_nom );
        if( res < 0 ) return res;
            //-- ��� �������� ������ ��������� �������
        if( !strcmp( condition, "active" )){
                //-- ���������� ��������� ������
            if( ! isActive(dbhadr) )  flag_cond = 1;
        } else {  //-- �������� �� ����������� ���������
                //-- ��������� ��������
            res = getFragmSize( dbhadr, fragm_size );
            if( fragm_size > buf_size ){
                delete[] fragm_buf;
                buf_size = fragm_size + 4096; //-- ������� ����
                fragm_buf = new char[buf_size];
            }
            res = getFragm( dbhadr, fragm_buf, buf_size );
                //-- �������� �������� ���������
            subst_count = substCount( fragm_buf );
            subst_offset = getSubstOffset( fragm_buf, subst_nom );
            subst_len = substLen( fragm_buf, subst_nom );
            pc_subst = (subst_offset < 0)? 0: fragm_buf + subst_offset;
            res = getSubint( value, ival, 0 );
            res = getSubint( fragm_buf, isub, subst_nom );
                //-- �������� �� ����������� ���������
            if( !strcmp( condition, "fill" )){
                    //-- ���������� ������ � ������ �����
                 if(( subst_len == 0 )||( subst_count <= subst_nom ))  flag_cond = 1;
            } else if( !strcmp( condition, "str=" )){
                if( !pc_subst || compSpec0(  pc_subst, value ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "str<>" )){
                if( pc_subst && !compSpec0(  pc_subst, value ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "*=*" )){
                if( pc_subst && checkSubst(  pc_subst, value ,0 ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "!*=*" )){
                if( pc_subst && !checkSubst(  pc_subst, value ,0 ) )  flag_cond = 1;
            } else if ( !strcmp( condition, "=" )){
                if( isub != ival ) flag_cond = 1;
            } else if ( !strcmp( condition, "<>" )){
                if( isub == ival ) flag_cond = 1;
            } else if ( !strcmp( condition, ">" )){
                if( isub <= ival ) flag_cond = 1;
            } else if ( !strcmp( condition, "<" )){
                if( isub >= ival ) flag_cond = 1;
            } else if ( !strcmp( condition, ">=" )){
                if( isub < ival ) flag_cond = 1;
            } else if ( !strcmp( condition, "<=" )){
                if( isub > ival ) flag_cond = 1;
            }
        }
            //-- ����� �������� �������
        if( flag_cond > 0){
            setRefIn( fhr, ref_nom, -fragm_nom );
        }
        else
            active_count++;
    };
    delete[] fragm_buf;
    return  active_count;
};
//----------------------------------------------------------

//== �������� ����� ����� �� ��������� � �������� �������
//-- ���������� ����� ��������� ��� ����� ��� ������
int   getSubint(char* pc, int& ires, int nom_sub){
    int     subst_offset = getSubstOffset(  pc, nom_sub );
    if(  subst_offset < 0 ) return  subst_offset;
    int     subst_len =  substLen( pc, nom_sub );
    if( subst_len == 0 ) { ires = 0; return 0; }
    int     num = 0;
    int     znak = 0;
    for( ;((*pc<0x30)||(*pc>0x39)); pc++) // find first dec digit
    {   if(*pc==0x2d) znak = 1; //-- look for minus
        if((!*pc)||(*pc == 0x0D)||(*pc == 0x0A)||(*pc == '|')||(!*pc)) return 0;
    }
    for( ;((*pc>=0x30)&&(*pc<=0x39)); pc++ )
        num = num*10 + (*pc-0x30);
    if( znak ) num = -num;
    ires = num;
    return  subst_len;
};
//----------------------------------------------------------


//== ������������ ������ � ������ �� ������ �����
//-- ���������� ����� �������������� ������ ��� ����� ��� ������

int  formStint( char* pcbuf, int buf_size, int ival ){
    if( !pcbuf ) return - BAD_PTR;
        //-- ������������ ������� ��������
    if( !ival ) { *pcbuf++ = '0';  *pcbuf = 0; return 1; }
        //-- ������� ��������� ������ ������
    char     znak = ' ';
    if( ival < 0 ){ ival = -ival; znak = '-'; }
    char* pc = pcbuf + buf_size - 1;
    *pc-- = 0;
    while( ival ) {
        if( pc == pcbuf)  return -BUFFER_SMALL;
        *pc-- = (char)(ival%10+0x30);
        ival /= 10;
    }
    *pc = znak;
        //-- ��������� ������ � ����� ����� ������
    int     sval_offset = pc - pcbuf;
    for( int j = 0; j < buf_size - sval_offset; j++ )
        *pcbuf++ = *pc++;
    return  buf_size - sval_offset;
};
//----------------------------------------------------------

//== �������� �������� ��������� � ������ �����
//-- ��������� �������� � ����� pcres
//-- ���������� ����� ���������� ��� ����� ��� ������
int  addSubint(char* pc, char* pcres, int res_size,  int ival) {
    char    temp_buf[20];
    int     res = formStint( temp_buf, 20, ival );
    if( res < 0 ) return  res;
    return  addSubst( pc, pcres, res_size, temp_buf );
};
//----------------------------------------------------------

//-- �������� �������� ��������� � ������ ����� ������
//-- ��������� ��������� � ����� pcres
//-- ���������� ����� ���������� ��� ����� ��� ������
int insSubint(char* pc, char* pcres, int res_size, int nom_sub, int ival) {
    char    temp_buf[20];
    int     res = formStint( temp_buf, 20, ival );
    if( res < 0 ) return  res;
    return  insSubst( pc, pcres, res_size, nom_sub, temp_buf );
};
//----------------------------------------------------------

//-- �������� ��������� �� ����� �������� �������� � ������ ����� ������
//-- ��������� ��������� � ����� pcres
//-- ���������� ����� ���������� ��� ����� ��� ������
int setSubint(char* pc, char* pcres, int res_size, int nom_sub,  int ival ) {
    char    temp_buf[20];
    int     res = formStint( temp_buf, 20, ival );
    if( res < 0 ) return  res;
    return  setSubst( pc, pcres, res_size, nom_sub, temp_buf );
};
//----------------------------------------------------------

//== ������������ ��������� ���� �� �������
//--  �������� �� ���� �������� ������� DBSN
//--  ��� ������ ������ ��������� �������
//--  � ���� ��� ����������� ��������� ������� ������ �� ��� ������
//--  � ��������� ����
//--  ���������� ����� ������ � ��������� �����
//--  ��� ������������� ��� ������
int  formDBRF(  int dbhadr, int fhr, int subst_nom, char* condition, char* value ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    ftruncate(fileno( (FILE*)fhr ), 0);  //-- �������� ��������� ����
    int     res = countFragmA( dbhadr );
    if( res < 0 ) return res;
    int     fragm_count = res;  //-- ����� ���������� � �������� �������
    int     fragm_nom = 0;
    int     ref_count = 0;  //-- ����� �������� ������
    int     buf_size = 4096;
    char*   fragm_buf = new char[buf_size];  //-- ����� ��� ���������
    int     fragm_size = 0;
    int     subst_count = 0;
    int     subst_offset = 0;
    int     subst_len = 0;
    int     isub = 0,   //-- ����� �������� ���������
            ival = 0;   //-- ����� �������� ���������
    char*   pc_subst = 0;
    for(  fragm_nom = 1; fragm_nom <= fragm_count; fragm_nom++ ) {
            //-- ���������� ��������� ������
        res = setNom( dbhadr, fragm_nom );
        if( res < 0 ) return res;
            //-- ����� ��������
        if( !strcmp( condition, "all" )){ //-- ����� ���� �������
            addRef(  fhr, fragm_nom );  ref_count++;  continue;
        }
            //-- ��������� �������� ������ ��� �������� �������
        if( !isActive(dbhadr) )  continue;
        if( !strcmp( condition, "active" )){ //-- ��� ��������
            addRef(  fhr, fragm_nom );  ref_count++;  continue;
        }
            //--��������� �������� � �����
        res = getFragmSize( dbhadr, fragm_size );
        if( fragm_size > buf_size ){ //--������������ ������ ������ ��� ��������
            delete[] fragm_buf;
            buf_size = fragm_size + 4096; //-- ������� ����
            fragm_buf = new char[buf_size];
        }
        res = getFragm( dbhadr, fragm_buf, buf_size );
            //-- �������� ��������� ���������
        subst_count = substCount( fragm_buf );
        subst_offset = getSubstOffset( fragm_buf, subst_nom );
        subst_len = substLen( fragm_buf, subst_nom );
        pc_subst = (subst_offset < 0)? 0: fragm_buf + subst_offset;
            //-- �������� �� ��������� ����
        if( !strcmp( condition, "fill" )){   //-- ���� �� �����
            if(( subst_len != 0 )&&( subst_count <= subst_nom )) {
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� ��������� ������ � ��������
        if( !strcmp( condition, "*=*" )){
            if( pc_subst && checkSubst(  pc_subst, value, 0 ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� ����������� ������ � ��������
        if( !strcmp( condition, "!*=*" )){
            if( pc_subst && !checkSubst(  pc_subst, value, 0 ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� ��������� ��������� ������
        if( !strcmp( condition, "str=" )){
            if( pc_subst && !compSpec0(  pc_subst, value ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� ����������� ��������� ������
        if( !strcmp( condition, "str<>" )){
            if( pc_subst && compSpec0(  pc_subst, value ) ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
        res = getSubint( value, ival, 0 );
        res = getSubint( fragm_buf, isub, subst_nom );
            //-- �������� �� �������� ��������� ��������� ������
        if( !strcmp( condition, "=" )){
            if( isub == ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� �������� ����������� ��������� ������
        if( !strcmp( condition, "!=" )){
            if( isub != ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� �������� ������
        if( !strcmp( condition, ">" )){
            if( isub > ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� �������� ������
        if( !strcmp( condition, "<" )){
            if( isub < ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� �������� ������ ��� �����
        if( !strcmp( condition, ">=" )){
            if( isub >= ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
            //-- �������� �� �������� ������ ��� �����
        if( !strcmp( condition, "<=" )){
            if( isub <= ival ){
                addRef(  fhr, fragm_nom );  ref_count++;  continue;
            }
        }
    };
    delete[] fragm_buf;
    return  ref_count;
};
//----------------------------------------------------------

//== ��������������� ������� ��������� ��������
//-- ���������� ����� ����� �������� ��������� ���� �����
//-- �� ��������� ������� condition
//-- ������� ����������, ����������
//-- 1  ���� ������� �����������
//-- 0  ���� �� �����������
//-- ��� ��������� ������� �������� "|" ������� ������ ������ ����
//-- 

int   compSubst(  char* pc1, char* pc2, int subst_nom, char* condition ) {
    int     subst1_offset = getSubstOffset( pc1, subst_nom );
    char*   pc_subst1 = pc1 + subst1_offset;
    int     subst2_offset = getSubstOffset( pc2, subst_nom );
    char*   pc_subst2 = pc2 + subst2_offset;
    if( !strcmp( condition, "str>" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) > 0 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "str>=" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) >= 0 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "str<" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) < 0 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "str<=" )){
        if( compSpec0(  pc_subst1, pc_subst2 ) <= 0 )  return 1;
        return 0;
    }
        //-- ���� ��������� ���������
    int     res = 0; 
    int     isub1 =0, isub2=0;
    res = getSubint( pc1, isub1, subst_nom );
    res = getSubint( pc2, isub2, subst_nom );
    if( !strcmp( condition, ">" )){
        if( isub1 > isub2 )  return 1;
        return 0;
    }
    if( !strcmp( condition, ">=" )){
        if( isub1 >= isub2 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "<" )){
        if( isub1 < isub2 )  return 1;
        return 0;
    }
    if( !strcmp( condition, "<=" )){
        if( isub1 <= isub2 )  return 1;
        return 0;
    }
    return  res;  //-- �� ��������� ����������
}


//== ������������� ��������� ���� �� �������
//--  �������� �� ���� �������� ������� DBSN
//--  ��� ������ ���� ������� ��������� �������
//--  ���������� ����� �������� ������� � ��������� �����
//--  ��� ������������� ��� ������
int  sortDBRF(  int dbhadr, int fhr, int subst_nom, char* condition ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     res = countFragmA( dbhadr );
    if( res < 0 ) return res;
    //int     fragm_count = res;  //-- ����� ���������� � �������� �������
    int     fragm1_nom = 0;
    int     fragm2_nom = 0;
    int     temp_nom = 0;
    int     ref_count = countRef( fhr );  //-- �����  ������
    int     del_count = 0;
    int     ref1_nom = 0;
    int     ref2_nom = 0;
    int     reftemp_nom = 0;
        //-- �������� � ����� ��������� ������
    for( ref1_nom = 1, ref2_nom = ref_count; ref1_nom < ref2_nom; ref1_nom++){
        fragm1_nom = getRef(  fhr, ref1_nom );
        if( fragm1_nom < 0 ){
                //-- �������� ������ ���� ����� � �����
            for( del_count++ ; ref2_nom > ref1_nom; ref2_nom--){
                fragm2_nom = getRefIn(  fhr, ref2_nom );
                if( fragm2_nom > 0 ) break;
            }
            if( fragm2_nom > 0 ) xchgRef( fhr, ref1_nom,  ref2_nom );
        }
    }
    ref_count -= del_count;
    if( ref_count <= 0 ) return 0;  //--������� ���� ��� �������� �������
    int     flag_sort = 0;   //-- ������� ������������� ����������
        //-- �������� �� ���������� �� ������� ����������
    if( !strcmp( condition, "nom+" )){ //-- �� ����������� �������
            //-- ��������� �������������
        for( ref1_nom = 1; ref1_nom < ref_count; ref1_nom++){
            fragm1_nom = getRef(  fhr, ref1_nom );
            fragm2_nom = getRef(  fhr, ref1_nom + 1 );
            if( fragm2_nom < fragm1_nom ){ flag_sort = 1; break;}
        }
        if( !flag_sort )  return ref_count;
            //--���������, �.�. ���������� ����������
        for( ref2_nom = ref_count; ref2_nom >1; ref2_nom--){
            fragm2_nom = getRef(  fhr, ref2_nom );
            temp_nom = fragm2_nom;  reftemp_nom = ref2_nom;
            for( ref1_nom = 1; ref1_nom < ref2_nom; ref1_nom++){
                    //-- ���� ������������
                fragm1_nom = getRef(  fhr, ref1_nom );
                if( fragm1_nom > temp_nom ) { temp_nom = fragm1_nom;  reftemp_nom = ref1_nom; }
            }
                //--���� ���� ��������� ������������
            if(  temp_nom > fragm2_nom )
                xchgRef( fhr, ref2_nom,  reftemp_nom );
        }
        return ref_count;
    }
    if( !strcmp( condition, "nom-" )){ //-- �� �������� �������
            //-- ��������� �������������
        for( ref1_nom = 1; ref1_nom < ref_count; ref1_nom++){
            fragm1_nom = getRef(  fhr, ref1_nom );
            fragm2_nom = getRef(  fhr, ref1_nom + 1 );
            if( fragm2_nom > fragm1_nom ){ flag_sort = 1; break;}
        }
        if( !flag_sort )  return ref_count;
            //--���������, �.�. ���������� ����������
        for( ref2_nom = ref_count; ref2_nom >1; ref2_nom--){
            fragm2_nom = getRef(  fhr, ref2_nom );
            temp_nom = fragm2_nom;  reftemp_nom = ref2_nom;
            for( ref1_nom = 1; ref1_nom < ref2_nom; ref1_nom++){
                    //-- ���� �����������
                fragm1_nom = getRef(  fhr, ref1_nom );
                if( fragm1_nom < temp_nom ) { temp_nom = fragm1_nom;  reftemp_nom = ref1_nom; }
            }
                //--���� ���� ��������� ������������
            if(  temp_nom < fragm2_nom )
                xchgRef( fhr, ref2_nom,  reftemp_nom );
        }
        return ref_count;
    }
    //== ���� ���������� �� ����������� ����������
        //--������� ������� ������ ������ �� ������������� ���������
    int     buf_size = 0, fragm_size = 0;
    for( ref1_nom = 1; ref1_nom <= ref_count; ref1_nom++){
        fragm1_nom = getRef(  fhr, ref1_nom );
        setNom( dbhadr, fragm1_nom );
        getFragmSize( dbhadr, fragm_size );
        if( fragm_size > buf_size ) buf_size = fragm_size;
    }
    buf_size += 16;  //-- ����� �� ������� ����
    char*   fragm1_buf = new char[buf_size];  //-- ����� ��� 1-�� ���������
    char*   fragm2_buf = new char[buf_size];  //-- ����� ��� 2-�� ���������
        //-- �������� ���� ���������
    for( ref2_nom = ref_count; ref2_nom >1; ref2_nom--){
        fragm2_nom = getRef(  fhr, ref2_nom );
        getFragmByRef( dbhadr, fhr, ref2_nom, fragm2_buf, buf_size );
        temp_nom = fragm2_nom;  reftemp_nom = ref2_nom;
        for( ref1_nom = 1; ref1_nom < ref2_nom; ref1_nom++){
                //-- ���� �����������
            fragm1_nom = getRef(  fhr, ref1_nom );
            getFragmByRef( dbhadr, fhr, ref1_nom, fragm1_buf, buf_size );
            if( compSubst(  fragm1_buf, fragm2_buf, subst_nom, condition )){
                temp_nom = fragm1_nom;  reftemp_nom = ref1_nom;
                strcpy( fragm1_buf, fragm2_buf);
            }
        }
            //--���� ���� ��������� ������������
        if(  temp_nom < fragm2_nom )
            xchgRef( fhr, ref2_nom,  reftemp_nom );
    }
    delete[] fragm1_buf;
    delete[] fragm2_buf;
    return  ref_count;
};
//----------------------------------------------------------

//== ���������, �������� �� ��������� subst_nom �������� templ
//-- ���������� 1 ���� templ �������
//-- ��� 0 ���� �� ����� templ � ���������
int   checkSubst(  char* pc, char* templ, int subst_nom ) {
    int     subst_offset = getSubstOffset( pc, subst_nom );
    char*   pc_subst = pc + subst_offset;
    int     subst_len = substLen( pc, subst_nom );
    int     templ_len = getLen( templ);
    char*   pc_loc;
    int     j=0;
        //-- ��������� ������ � ������� �������
    for( j = 0; j < templ_len; j++) {
        templ[j] =  toupper( templ[j] );
    }
        //--  ��������� �� ���� ��������� ��������
    for( int pos = 0;  pos <= subst_len - templ_len;  pos++) {
        pc_loc = pc_subst+pos;
        for( j = 0; j< templ_len; j++ ){
            if( toupper(pc_loc[j]) != templ[j] )  break;
        }
        if( j == templ_len )
            return  1;   //-- ����� ����������
    }
    return 0;
}

//==========================================================
//== 17 mar 2014 ==
//== ����������� ���� ������ ��� ������ ������
//--  ���������� ���������� ������������ �������
//--  ��� 0 ��� ������
//-- ���  flag_pack != 0 ��������� ������ �� ����������

int    copyDBRF( char*  fil_ref, char*  copy_ref, int flag_pack ) {
        //-- ��������� �������� � �������������� �����
    int     fhr = openDBRF( fil_ref );
    if( fhr < 0 ) return  fhr;
    int     res = createDBRF( copy_ref );
    if( res < 0 ) return res;
    int     new_fhr = openDBRF( copy_ref );
    if( new_fhr < 0 ) return new_fhr;
        //-- �������� � ����� ���� ������
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    int     fragm_nom = 0;
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fragm_nom = getRefIn( fhr, ref_nom );
        if( !flag_pack ) //-- ���������� ��� ������
            addRefIn(  new_fhr, fragm_nom );
        else {
            if( fragm_nom > 0 )
                addRefIn(  new_fhr, fragm_nom );
        }
    };
        //-- ��������� ����� ������
    res = closeDBRF( fhr );
    ref_count = countRefIn( new_fhr );
    res = closeDBRF( new_fhr );
    if( res < 0 ) return res;
    return  ref_count;
};

//== ��������� ���� ������
//--  ��������� ������ ������ � �������������� �������� ����������
//--  ���������� ����� ������� � ���������� �����
int  packDBRF(  int fhr  ){
        //-- �������������� ����������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     new_refnom = 0;
    int     ref_count = ftell( (FILE*)fhr ) /4;
    int     fragm_nom = 0;
    for( int ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fragm_nom = getRefIn( fhr, ref_nom );
        if( fragm_nom > 0 ){
            new_refnom++;
            setRefIn(  fhr, new_refnom, fragm_nom );
        }
    };
        //-- ��������� ������ ����� ������ � ���������  ���
    ftruncate(fileno( (FILE*)fhr ), new_refnom*4 );
    ref_count = ftell( (FILE*) fhr ) /4;
    int     res = closeDBRF( fhr );
    if( res < 0 ) return res;
    return  ref_count;
};
//----------------------------------------------------------

//== ������������ ����� ������� ��� ������� �� �������
//--  ������� ����� �������,
//--  �������� �� ����� ���������� ����� DBRF
//--  � �������� � ����� ������� ������ �������� �������� ��������� DBSN
//--  ���������� ����� ������� � ����� �������
//--  ��� ������������� ��� ������
int  selectDBSN(  int dbhadr, int fhr, char* new_tabl ){
        //-- ��������� ��������
    if( !dbhadr )   return 0;
    int*    dbh = ( int* )dbhadr;
    if( dbh[3] > FATAL_ERROR)    return  -dbh[3];
    dbh[3] = 0; //-- ������� ��������������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
        //-- ������� ����� �������
    int     res = createDBSN( new_tabl);
    if( res < 0 ) return res;
    res = openDBSNA( new_tabl );
    int     new_dbhadr = res;
    //== ����������� ����� ������ ���������
    int     ref_nom,
            ref_count =  countRef( fhr );
    int     fr_nom = 0,
            fr_count = 0;
    int     fr_size = 0,
            max_frsize = 0,
            buf_size = 4096;
        //-- �������� �� ���� �������
    for(  ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fr_nom = getRefIn( fhr, ref_nom );
        if( fr_nom > 0 ){ //-- ������ ��� �������� ������
            setNom( dbhadr, fr_nom );
            getFragmSize( dbhadr, fr_size );
            if( fr_size > max_frsize )  max_frsize = fr_size ;
            fr_count++;
        }
    }
    if( !fr_count ) { //-- ��� �������� ������
        close( new_dbhadr );
        return 0;
    }
        //-- ��������� ����� ���������
    if( buf_size <= max_frsize ) buf_size = max_frsize + 16;
    char    *fragm;
    fragm   = new char[buf_size];
    //=== �������� � ����� ������� ��� �������� ���������
    for(  fr_count=0, ref_nom = 1; ref_nom <= ref_count; ref_nom++ ) {
        fr_nom = getRefIn( fhr, ref_nom );
        if( fr_nom > 0 ){ //-- ������ ��� �������� ������
            setNom( dbhadr, fr_nom );
            getFragmSize( dbhadr, fr_size );
            if( fr_size > 0 ) {
               res = getFragm( dbhadr, fragm, buf_size );
               addFragm( new_dbhadr, fragm );
               fr_count ++;
            }
        }
    }
    closeDBSN( new_dbhadr );
    return  fr_count;
};
//----------------------------------------------------------

//== �������� � ��������� ����� ����� ��������� ����� �������� �������
//--  ���������� ����� ����� ������ ��� ������
//--  ��� ������������� ��� ������
//--  ���� �� �������������� ��������������
int  insRefAfter(  int fhr, int ref_nom, int fragm_nom ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    fseek((FILE*)fhr,0,2);
    int     ref_count = ftell( (FILE*)fhr ) /4;
    if(( ref_nom < 1)||(ref_nom > ref_count)) return -OUT_OF_RANGE;
    if( ref_nom == ref_count )
        return  addRef(  fhr, fragm_nom );
        //-- �������� ������� � ������� ����� �����
    int     frnom_loc;
    int     refnom_loc;
    for( refnom_loc = ref_count; refnom_loc > ref_nom; refnom_loc--) {
        frnom_loc = getRefIn( fhr, refnom_loc );
        setRefIn( fhr, refnom_loc + 1, frnom_loc );
    }
        //-- ���������� � ������ ����� ����� ��������
    setRefIn( fhr, ref_nom+1, fragm_nom );
    return  ref_nom + 1; //-- ����� ������ �� ���������� �������
}
//----------------------------------------------------------

//== ����������� ������ ref_nom2 �� ����� ref_nom1 �� �������
//--  ���������� ����� ����� ������ ( ref_nom1 ) ��� ������
//--  ��� ������������� ��� ������
//--  ���� �� �������������� ��������������
int  moveRef(  int fhr, int ref_nom1, int ref_nom2 ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom1 < 1)||(ref_nom1 > ref_count)) return -OUT_OF_RANGE;
    if(( ref_nom1 < 2)||(ref_nom2 > ref_count)) return -OUT_OF_RANGE;
    if(ref_nom1 == ref_nom2 ) return ref_nom1; //--�����������

    int     frnom_tmp = getRefIn( fhr, ref_nom1 );  //-- ��������� ��� ��������
    int     frnom_loc;
    int     refnom_loc;

    if(ref_nom1 < ref_nom2 ){  //-- ���� �������� ������
        for( refnom_loc = ref_nom2; refnom_loc > ref_nom1; refnom_loc--) {
            frnom_loc = getRefIn( fhr, refnom_loc-1 );
            setRefIn( fhr, refnom_loc, frnom_loc);
        }
    } else { //-- ���� �������� �����
        for( refnom_loc = ref_nom2; refnom_loc < ref_nom1; refnom_loc++) {
            frnom_loc = getRefIn( fhr, refnom_loc+1 );
            setRefIn( fhr, refnom_loc, frnom_loc);
        }
    }
    setRefIn( fhr, ref_nom1, frnom_tmp );
    return ref_nom1;
}
//----------------------------------------------------------


//== ������� ���� ������ �� ��������� �������
//--  ���� �� ref_count ������ ������� � ref_nom ������������
//--  ��������� �������� 0�80000000, 0�00000000
//--  ��� ���������������� ��� ��������� ����(���� - �������)
//--  ���������� ����� ������ �� ��������� ������ ( ref_nom ) ��� ������
//--  ��� ������������� ��� ������
//--  ���� �� �������������� ��������������
int  pushRblock(  int fhr, int ref_nom, int block_len ){
    //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    if(( ref_nom < 1)||(block_len < 0 )||(ref_nom + block_len > countRefIn( fhr ))) return -OUT_OF_RANGE;
    insRefIn( fhr, ref_nom + block_len, END_BRACKET );
    insRefIn( fhr, ref_nom, BEG_BRACKET);
    return  ref_nom;
}
//----------------------------------------------------------

//== ����� ������ �����( ����������� ������ )
//-- ���������� ����� ������ �� ����������� ������
int  findRblock(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    while( BEG_BRACKET != getRefIn( fhr, ref_nom )){
        ref_nom++;
        if( ref_nom > ref_count ) return -OUT_OF_RANGE;
    }
    return ref_nom;
}

//== ����� ��������������� ����������� ������
//-- ���������� ����� ������ �� ��������������� ����������� ������
int  endRblock(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    if( BEG_BRACKET != getRefIn( fhr, ref_nom ))  return - NOT_A_BRACKET;
    int     bracket_count = 1;  //-- ����� ������
        //-- ���� ��������������� ����������� ������
    for( ref_nom++; ref_nom <= ref_count; ref_nom++){
        if( BEG_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count++;
        if( END_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count--;
        if( !bracket_count)  break;
    }
    if(ref_nom > ref_count ) return -OUT_OF_RANGE;
    return  ref_nom;
}

//== ����� ��������������� ����������� ������ (�����)
//-- ���������� ����� ������ �� ��������������� ����������� ������
int  begRblock(  int fhr, int ref_nom ){
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    if( END_BRACKET != getRefIn( fhr, ref_nom ))  return - NOT_A_BRACKET;
    int     bracket_count = 1;  //-- ����� ������
        //-- ���� ��������������� ����������� ������
    for( ; ref_nom >=1; ref_nom--){
        if( END_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count++;
        if( BEG_BRACKET == getRefIn( fhr, ref_nom ))
            bracket_count--;
        if( !bracket_count)  break;
    }
    if(ref_nom <1 ) return -OUT_OF_RANGE;
    return  ref_nom;
}

//== ������ ���� ������ �� ���������� �������
//--  � �������� ��������� ref_nom ����������� �� BEG_BRACKET
//--  ���� ��� �� ��� - ���������� - NOT_A_BRACKET
//--  ����� ���� ��������������� �� ����������� ������
//--  � �������� �� ����� ������ ��� ��� ������
//--  ��� ���������������� ��� �������� ���������� �����(���� - �������)
//--  �� ������� ����
//--  ���������� ����� ������ �� ������ ��������� ������ ( ref_nom ) ��� ������
//--  ��� ������������� ��� ������
int  popRblock(  int fhr, int ref_nom ){
        //-- ��������� ������
    int     end_nom = endRblock( fhr, ref_nom );
    if( end_nom < 0 ) return end_nom;
        //-- ������� ����������� � ��������� ������
    cutRefIn( fhr, end_nom );
    return  cutRefIn( fhr, ref_nom );
}
//----------------------------------------------------------

//== �������� ���� �� ����� ������
//--  ���� �� block_len ������ ������� � ref_nom ������������
//--  ���������� ref_nom ��� ref_nom-1  ���� ��� ��� ����� �����
//--  ��� ������������� ��� ������
int  cutRblock(  int fhr, int ref_nom, int block_len ){
        //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_count < 0 )||(ref_nom + block_len > ref_count )) return -OUT_OF_RANGE;
        //-- ���������� �����
    if( ref_nom + block_len == ref_count ) {
        ref_nom--;  //--  ������ ��������� ������ ����� ������
        ftruncate(fileno( (FILE*)fhr ), ref_nom*4 );
        return  ref_nom;
    }
        //-- �������� ��������
    int     fragm_nom;
    for( int ref_tmp = ref_nom; ref_tmp < ref_count - block_len; ref_nom++) {
        fragm_nom = getRefIn( fhr, ref_tmp + block_len);
        setRefIn( fhr, ref_tmp, fragm_nom );
    }
    return  ref_nom;
}
//----------------------------------------------------------

//== ����������� ���� ������ � ����� ������� �����
//--  ���� �� block_len ������ ������� � ref_nom ������������
//--  ���������� ref_nom ��� ref_nom-1  ���� ��� ��� ����� �����
//--  ��� ������������� ��� ������
int  moveRblock(  int fhr, int new_nom, int ref_nom, int block_len ){
        //-- ��������� ������
    if( fhr <= 0 ) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_count < 0 )||(ref_nom + block_len > ref_count )) return -OUT_OF_RANGE;
    if(( new_nom < 1)||(new_nom + block_len > ref_count )) return -OUT_OF_RANGE;
    if( new_nom == ref_nom ) return  new_nom; //-- ����������� ������
        //-- ������ ������� ���� � ������������
    if(( new_nom > ref_nom )&&(new_nom < ref_nom + block_len )) return -OUT_OF_RANGE;
        //-- ��������� ���������� ����
    int     nom;
    int*    fragm_arr = new int[ block_len];
    for( nom = 0; nom < block_len; nom++)
        fragm_arr[ nom ] = getRefIn( fhr, ref_nom + nom);
        //-- ���������� ����� �����
    int     ref_tmp;
    int     fragm_nom;
    if( new_nom < ref_nom  ) {
            //-- �������� ������
        for( ref_tmp = ref_nom-1; ref_tmp >= new_nom; ref_tmp-- ) {
            fragm_nom = getRefIn( fhr, ref_tmp );
            setRefIn( fhr, ref_tmp+block_len, fragm_nom );
        }
    } else {  //-- ���������� ����� �����
        for( ref_tmp = ref_nom + block_len; ref_tmp < new_nom + block_len; ref_tmp++ ) {
            fragm_nom = getRefIn( fhr, ref_tmp );
            setRefIn( fhr, ref_tmp-block_len, fragm_nom );
        }
    }
       //-- ��������� ����������� ����
    for( nom = 0 ; nom < block_len; nom++ ){
        fragm_nom = fragm_arr[ nom ];
        setRefIn( fhr, new_nom + nom, fragm_nom );
    }
    delete[] fragm_arr;
    return  new_nom;   
}
//----------------------------------------------------------

//== �������� ���� ������ �� ������� �����
//--  ���� �� block_len ������  �� fhr2 ������� � ref_nom2 ������������
//--  ����������� � ����� ��������� ����� ������
//--  ���������� ����� ����� ������� � fhr
//--  ��� ������������� ��� ������
int  addRblock(  int fhr, int fhr2, int ref_nom2, int block_len ){
        //-- ��������� ������
    if(( fhr <= 0 )||( fhr2 <= 0 )) return -DBR_NOT_EXIST;
    int     ref_count2 = countRefIn( fhr2 );
    if(( ref_nom2 < 1)||(block_len < 1 )||(ref_nom2 + block_len > ref_count2 )) return -OUT_OF_RANGE;
        //-- ��������� ������
    int     fragm_nom;
    for(int nom = 0; nom < block_len; nom++ ){
        fragm_nom = getRefIn( fhr2, ref_nom2 + nom);
        addRefIn( fhr, fragm_nom);
    }
    return  countRefIn( fhr);
}
//----------------------------------------------------------

//== �������� ���� ������ �� ������� �����
//--  ���� �� block_len ������  �� fhr2 ������� � ref_nom2 ������������
//--  ����������� � ������ ����� ��������� ����� ������
//--  ���������� ����� ����� ������� � fhr
//--  ��� ������������� ��� ������
int  insRblock(  int fhr, int ref_nom,int fhr2, int ref_nom2, int block_len ){
        //-- ��������� ������
    if(( fhr <= 0 )||( fhr2 <= 0 )) return -DBR_NOT_EXIST;
    int     ref_count = countRefIn( fhr );
    if(( ref_nom < 1)||(ref_nom > ref_count )) return -OUT_OF_RANGE;
    int     ref_count2 = countRefIn( fhr2 );
    if(( ref_nom2 < 1)||(block_len < 1 )||(ref_nom2 + block_len > ref_count2 )) return -OUT_OF_RANGE;
        //-- ��������� ������
    int     fragm_nom;
    for(int nom = 0; nom < block_len; nom++ ){
        fragm_nom = getRefIn( fhr2, ref_nom2 + nom);
        insRefIn( fhr, ref_nom + nom, fragm_nom);
    }
    return  ref_nom;
}
//----------------------------------------------------------


//==  �������� ����� � ������ ������ ���� DBSN
//--  ���������� ����� ������
//--  ��� -��� ��������� ������
//--  ��� 0 ��� ������ ��� ������
int  writeFragm(int dbhadr, char* fragm, int fragm_nom ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int  res = setNomIn( (int *) dbhadr, fragm_nom );
    if( res < 0 )  return res;
    return  setFragm(  dbhadr, fragm );
}
//----------------------------------------------------------


//== ������� ������� �������� ���� DBSN  ===================
//--  ���������� ����� ������ ���
//--  ��� ������������� ��� ������
//--  ��� 0 ��� ������ ������

//-- ���������� ������� (���� ��� ��������� ������)
int  readFragm( int dbhadr, char* fragm, int bufsize, int fragm_nom ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    int  res = setNomIn( (int *) dbhadr, fragm_nom );
    if( res < 0 )  return res;
    return  getFragm( dbhadr, fragm, bufsize );
};
//----------------------------------------------------------

//==  ��������� ����� �������� ������� � ������� DBSN
//--  ���������� ����� ���������� �������� �������
//--  ��� ������������� ��� ��������� ������

//-- ���������� ������� (���� ��� ��������� ������)
int countFragmIn( int* dbh ) {
    dbh[3] = 0; //-- ������� ��������������
    //-- ����������� ����������� ������
    int     fhn = dbh[0];
    fseek((FILE*)fhn,0,2);
    int     rec_count = ftell( (FILE*)fhn ) /8;
    if( !rec_count )
        dbh[3] = DBN_IS_EMPTY;

    int     active_count = 0,  rec_size;
    for( int rec_nom = 1; rec_nom <= rec_count; rec_nom++) {
    //-- ��������� ���������� ��������� ���������
        fseek( (FILE*)fhn, (  rec_nom-1)*8+4, 0);
        rec_size = freadInt(   fhn );
        if(  rec_size > 0) active_count++;
    }
    return  active_count;
}

//-- ������������� ������� ---
int countFragm( int dbhadr ) {
    if( getErrCod(dbhadr) >= FATAL_ERROR ) return  - getErrCod(dbhadr);
    return  countFragmIn( (int*) dbhadr );

};
//----------------------------------------------------------

//== ������� ������� ���������� DBSN
//--  ���������� ���������� ������� ��� ������
//--  (����� ������� ���������� )
//--  � ������������� ����� ������ �� ������ ��������
//--  ��� ������������� ��� ������
int openDBSN( char* fil_name ) {
    int     res = openDBSNA( fil_name );
    if( res < 0 ) return  res;
    int     dbhadr = res;
    res = gotoFirst( dbhadr );
    if( res < 0 ) return  res;
    return  dbhadr;
}
//----------------------------------------------------------

//==  ������� ������ � ���������� ��������� ���������
//--    ���������� ����� ���������
//--    ��� -��� ��������� ������
//--    ��� 0 ��� ������������� ( ��� ������� �������� ������� )
int    gotoNext( int dbhadr ){
    return  goForward( dbhadr, 1);
}
//----------------------------------------------------------

//==  ������� ����� � ����������� ��������� ���������
//--    ���������� ����� ���������
//--    ��� -��� ��������� ������
//--    ��� 0 ��� ������������� ( ��� ������� �������� ������� )
int    gotoPrev( int dbhadr ){
    return  goBack( dbhadr, 1);
}
//----------------------------------------------------------

//-- �������� ������� ��������� 2� ����� � ������������� ��������
//-- 1  ���� subst(pc,0) > subst(pc1)
//-- 0  ���� ��� �����
//-- -1 ���� subst(pc,0) < subst(pc1)
//-- ��������������� �� 0 ��� "|" ��� ����� �� �����
long    compSpec0( char* pc, char* pc1){
    unsigned  char  bt, bt1;
    while( 1 ){
        bt = *pc++;  bt1 = *pc1++;
        //-- ������������ ����� �����
        if((( bt == 0 )||( bt == '|'))&&(( bt1 == 0 )||( bt1 == '|')))  return 0;
        if(( bt == 0 )||( bt == '|'))    return -1;
        if(( bt1 == 0 )||( bt1 == '|'))  return  1;
        //-- ��������� ��� ����� ��������
        bt = toupper(bt);  bt1 = toupper(bt1);
        if( bt == bt1 )  continue;
        if( bt > bt1 )   return  1;
        if( bt < bt1 )   return -1;
    }
};
//----------------------------------------------------------

////////////////////////////////////////////////////////////
/*  ����� �������, ������� ����������
      int > 0  ��� ���������� ���
      int = 0  ��� ���������
      int < 0  ��� ������
*/
//== ������� ������������ ����� ��������� ���� DBSN
int    getMaxFragmSize( int  dbhadr ){
    int     max_size = 0;
    int     rec_size = 0;
    int     id = gotoFirst( dbhadr );
    while( id > 0 ) {
        getFragmSize( dbhadr, rec_size );
        if( rec_size > max_size )
            max_size = rec_size;
        id = gotoNext( dbhadr );
    }
    return  max_size;
}
//----------------------------------------------------------

//== ��������� ��������� �������� ���� DBSN
int     getNextFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoNext( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//== ��������� ����������� �������� ���� DBSN
int     getPrevFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoPrev( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//== ��������� ������ �������� ���� DBSN
int     getFirstFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoFirst( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//== ��������� ��������� �������� ���� DBSN
int     getLastFragm(  int dbhadr, char* fragm, int bufsize ){
    int     id = gotoLast( dbhadr );
    if( id > 0 ){
        return  getFragm(  dbhadr, fragm, bufsize );
    }
    *fragm = 0;
    return  id;
}
//----------------------------------------------------------

//-- ������� ���  ������ ��������� ������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int cutRightEmpty(char* pc ) {    
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;    
    int     st_len = getLen( pc);
    if( st_len <1 )  return 0;
    char    *pc_src = pc + st_len;
        //-- ���������� ��� ������ ��������� ������
    for( ; st_len > 1; st_len--){
        pc_src--;
        if(  *pc_src =='|' )  continue;
        break;
    }
        //-- ������� ������ �����
    pc_src[1] = 0;
    return  st_len;
};
//----------------------------------------------------------
//-- ������� ���  ������ � ���������� ��������� ������
//-- ���������� ����� ������������ ������ ��� ����� ��� ������
int cutRightWhite(char* pc ) {    
        //-- ��������� ��������
    if( !pc ) return - BAD_PTR;    
    int     st_len = getLen( pc);
    if( st_len <1 )  return 0;
    char    *pc_src = pc + st_len;
        //-- ���������� ��� ������ � ���������� ��������� ������
    for( ; st_len > 1; st_len--){
        pc_src--;
        if(  *pc_src =='|' )  continue;
        if(  *pc_src ==' ' )  continue;
        break;
    }
        //-- ������� ������ �����
    pc_src[1] = 0;
    return  st_len;
};
//----------------------------------------------------------

