#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
processresult(MYSQL *mysql, MYSQL_RES *res)
{
MYSQL_ROW row;
MYSQL_FIELD *field;
int i, j, fldcnt, len;

fldcnt = mysql_num_fields(res);
printf("|");
mysql_field_seek (res, 0);
for (i = 0; i < fldcnt; i++) {
field = mysql_fetch_field (res);
len = strlen (field->name);
if (len < 4 ) len = 4;

if (len > field->max_length)
field->max_length = len;
printf (" %-*s |", (int) field->max_length, field->name);
}
printf("\n");

printf("+");
mysql_field_seek (res, 0);
for (i = 0; i < fldcnt; i++) {
field = mysql_fetch_field (res);
for (j = 0; j < field->max_length + 2; j++)
fputc('-', stdout);
printf("+");
}
printf("\n");
while ((row = mysql_fetch_row(res))) {
printf("|");
mysql_field_seek (res, 0);
for (i = 0; i < fldcnt; ++i) {
field = mysql_fetch_field (res);
printf(" %-*s |", (int) field->max_length,
row[i] != NULL ? row[i] : "NULL");
}
	printf("\n");
	}
}
int main()
{
	MYSQL mysql;
	mysql_init(&mysql);

	if(!mysql_real_connect(&mysql, "localhost",
"root", "4523", "ex9", 0, (char *)NULL, 0)){
	printf("MySQL Error %d : %s\n",
mysql_errno(&mysql), mysql_error(&mysql));
	exit(1);
}
while(1){

	char buf[128];
	MYSQL_RES *res;

	printf("mysql> ");
	if(fgets(buf, sizeof(buf), stdin) == NULL)	
	break;
	if(strcmp(buf, "quit\n")==0) break;

	if(mysql_query(&mysql, buf)) {

	printf("MySQL Error %d:%s \n",
mysql_errno(&mysql), mysql_error(&mysql));
	exit(1);
	}

	res = mysql_store_result(&mysql);
	if(res){
	processresult(&mysql,res);
	mysql_free_result(res);
	}
	else {
	if(mysql_field_count(&mysql)==0)
	printf("%lu rows affected\n",(unsigned long)mysql_affected_rows(&mysql));
	else{
	printf("MySQL Error %d: %s\n", mysql_errno(&mysql));
	exit(1);
	}
}
}
	mysql_close(&mysql);
	return 0;
}
