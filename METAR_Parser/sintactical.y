%{
	#include <stdio.h>
	void yyerror(const char* msg){
		fprintf(stderr, "%s\n", msg);
	}
%}

%union{
	char* text;
}

%type <text> reportA report

%token <text> TEXT_
%token <text> METAR_TEXT_ AP_ICAO_ DATE_GEN_ AUTO_GEN_

%%

report: METAR_TEXT_ AP_ICAO_ DATE_GEN_ reportA
		{
			fprintf(stdout,"Report: %s %s\n", $2, $3, $4);
		}
	|	METAR_TEXT_ AP_ICAO_ DATE_GEN_ AUTO_GEN_ reportA
		{
			fprintf(stdout, "Report: %s %s %s %s\n", $2, $3, $4, $5);
		}
		;

reportA: ;