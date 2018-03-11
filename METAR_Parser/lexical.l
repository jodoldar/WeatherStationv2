%{
	#include <stdio.h>
	#include "sintactical.h"

	void print_token(char* text);

	int verbose = 1;
%}

%option yylineno
delimiter	[ \t\n]+
char_minus	[a-z]
char_mayus	[A-Z]
number		[0-9]
char 		({char_mayus}|{char_minus})
ap_name		{char_mayus}{4}
num2		{number}{2}
num3		{number}{3}
num4		{number}{4}
date		{num2}{num4}"Z"
wind_unit	("KMH"|"KT"|"MPS")
wind_w_gust		{num3}{num2}"G"{num2}{wind_unit}
wind_wo_gust	{num3}{num2}{wind_unit}
variable_wind	{num3}"V"{num3}
wind 		({wind_w_gust}|{wind_wo_gust})({delimiter}{variable_wind})?
cloud_amount	("FEW"|"SCT"|"BKN"|"OVC")
cloud_type		("CB"|"TCU")
cloud1		{cloud_amount}{num3}({num3})*
cloud 		({cloud1})
temperature	{num2}"/"{num2}
pressureQ	"Q"{num4}
pressureA	"A"{num4}
pressure 	({pressureQ}|{pressureA})
change_type	("AT"|"FM"|"TL")
change_time	{change_type}{num4}
trend_type	("TEMPO"|"BECMG")
trend_large	{trend_type}{delimiter}{change_time}{delimiter}{wind}{delimiter}{num4}{delimiter}{num2}{delimiter}{cloud}
trend 		("NOSIG"|{trend_large})

%%

{delimiter}		{}
"METAR"			{print_token("METAR report-> ");return(METAR_TEXT_);}
"AUTO"			{print_token("Automatic generated-> ");return(AUTO_GEN_);}
{ap_name}		{print_token("Airport name-> ");return(AP_ICAO_);}
{num2}			{print_token("2-digit number-> ");}
{num3}			{print_token("3-digit number-> ");}
{num4}			{print_token("4-digit number-> ");}
{date}			{print_token("Date-> ");return(DATE_GEN_);}
{wind}			{print_token("Wind-> ");}
{cloud}			{print_token("Clouds-> ");}
{temperature}	{print_token("Temperature-> ");}
{pressure}		{print_token("Pressure-> ");}
{trend}			{print_token("Trending-> ");}
.				{printf("Text unrecognizable: %s \n", yytext);}
%%

void print_token(char* text){
	printf("Token:%s %s \n", text, yytext);
}