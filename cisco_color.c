/*
Copyright 2011 Thomas St.Pierre. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Thomas St.Pierre ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Thomas St.Pierre OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Thomas St.Pierre.
*/

#include <stdio.h>
#include <regex.h>
#include <string.h>

#define MAXMATCH 20 

/* Terminal Color codes */
#define TERM_BRIGHT_GREEN 	"\033[1m"
#define TERM_GREEN		"\033[1;32m"
#define TERM_BOLD_RED		"\033[1;31m"
#define TERM_RED		"\033[31m"
#define TERM_PURPLE		"\033[35m"
#define TERM_BOLD_PURPLE	"\033[1;35m"
#define TERM_BRIGHT_YELLOW	"\033[1;33m"
#define TERM_YELLOW		"\033[33m"
#define TERM_WHITE		"\033[37m"
#define TERM_BLUE		"\033[34m"
#define TERM_LIGHT_BLUE		"\033[1;34m"
#define TERM_CYAN		"\033[36m"

#define TERM_BG_GREEN		"\033[42m"
#define TERM_WARNING		"\033[41m\033[1m"

#define TERM_NONE		"\033[0m"

/* 
	Define our rules!
	Rules Are defined as a struct, first variable is a regex. The entire line is used to match
	context of what you want to color. Within the regex you use sub-expressions to define 
	what to color. For example with the regex "This is (some) context", it will match the
	entire line, but only the word 'some' will be colored. Second variable is an array which
	contains strings that will be inserted right before every matched sub-expression. (usually
	terminal escapes codes, but could be any string)

	Last variable is used internally to store compiled regex's
*/

struct color_match{
                const char *pattern;
                const char *insert_before[MAXMATCH];
                regex_t compiled;
        } patterns [] = {
/* Logs */
{"^\\*?([A-Z][a-z]{2}[ 0-9]{3}[0-9 ]{0,5} [0-9]{1,2}:[0-9]{2}:[0-9]{2}\\.[0-9]{3} [A-Z]{3}): (%[A-Z0-9_-]+:) (.*)$", {TERM_CYAN, TERM_RED, TERM_BLUE}},

/* Interface Name */
{"^(interface) ([a-zA-Z-]+)([0-9./]+)",{TERM_YELLOW, TERM_CYAN, TERM_BOLD_PURPLE}},

/* Interface parameters */
{"^ (description) (.*)$", {TERM_YELLOW, TERM_BLUE}},
{"^ (ip address) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}) (secondary)",
        {TERM_YELLOW, TERM_PURPLE, TERM_PURPLE, TERM_BRIGHT_GREEN}},
{"^ (ip address) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}).$",
	{TERM_YELLOW, TERM_PURPLE, TERM_PURPLE}},
{"^ (ip vrf forwarding) ([a-zA-Z0-9_-]+).$", {TERM_YELLOW,TERM_RED}},
{"^ (ip nat) (inside|outside).$", {TERM_YELLOW,TERM_RED}},
{"^ (encapsulation dot1Q) ([0-9]{1,4}).$", {TERM_YELLOW, TERM_PURPLE }},
{"^ (shutdown).$", {TERM_RED}},

/* Interface Status */
{"is (up), line protocol is (down)", {TERM_BRIGHT_GREEN, TERM_BOLD_RED}},
{"is (down), line protocol is (down)", {TERM_BOLD_RED, TERM_BOLD_RED}},
{"is (up), line protocol is (up)", {TERM_BRIGHT_GREEN, TERM_BRIGHT_GREEN}},
{"is (administratively down), line protocol is (down)", {TERM_YELLOW, TERM_YELLOW}},
{"is ([a-z]+), line protocol is ([a-z]+)", {TERM_WARNING, TERM_WARNING}},

/* Interface speeds */
{"^  [0-9]{1,2} [a-z]{6} [a-z]{5,6} rate ([0-9]{1,3})([0-9]{3})([0-9]{3})([0-9]{3}) bits.*$", {TERM_PURPLE, TERM_CYAN, TERM_BLUE, TERM_CYAN}},
{"^  [0-9]{1,2} [a-z]{6} [a-z]{5,6} rate ([0-9]{1,3})([0-9]{3})([0-9]{3}) bits.*$", {TERM_CYAN, TERM_BLUE, TERM_CYAN}},
{"^  [0-9]{1,2} [a-z]{6} [a-z]{5,6} rate ([0-9]{1,3})([0-9]{3}) bits.*$", {TERM_BLUE, TERM_CYAN}},
{"^  [0-9]{1,2} [a-z]{6} [a-z]{5,6} rate ([0-9]{1,3}) bits.*$", {TERM_CYAN}},



/* Hostname / config prompts */
{"^([a-zA-Z0-9\\._\\-]*)(\\(config)(-[a-z_-]+)(\\))#",{TERM_BRIGHT_GREEN,TERM_RED,TERM_PURPLE,TERM_RED}},
{"^([a-zA-Z0-9\\._\\-]*)(\\(config\\))#",{TERM_BRIGHT_GREEN,TERM_RED}},
{"^([a-zA-Z0-9\\._-]*)#",{TERM_BRIGHT_GREEN}},
{"^([a-zA-Z0-9\\._-]*)>",{TERM_BRIGHT_GREEN}},

/* OSPF config */
{"^(router ospf) ([0-9]+) (vrf) ([a-zA-Z_-]+).$", {TERM_YELLOW, TERM_PURPLE, TERM_YELLOW, TERM_RED}},
{"^(router ospf) ([0-9]+).$", {TERM_YELLOW, TERM_PURPLE}},
{"^( log-adjacency-changes).$", {TERM_YELLOW}},
{"^ (redistribute) ([a-zA-Z0-9 ]+) (subnets).$", {TERM_YELLOW, TERM_PURPLE, TERM_YELLOW}},
{"^ (network) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3} [0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}) (area) ([0-9]+).$",
						{TERM_YELLOW, TERM_PURPLE, TERM_YELLOW, TERM_RED}},
{"^ (default-information) (originate)(.*)$", { TERM_YELLOW, TERM_RED, TERM_PURPLE}},

/* BGP Config */
{"^(router bgp) ([0-9]+).$", {TERM_YELLOW, TERM_PURPLE}},
{"^ (address-family) ([a-z0-9]+) (vrf) ([a-zA-Z0-9_-]+).$", {TERM_YELLOW, TERM_PURPLE, TERM_YELLOW, TERM_RED}},
{"^ (address-family) ([a-z0-9]+).$", {TERM_YELLOW, TERM_PURPLE}},
{"^ {1,2}(no) (synchronization).$", {TERM_RED, TERM_YELLOW}},
{"^ {1,2}(neighbor) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}) (remote-as) ([0-9]+).$",
					{TERM_YELLOW, TERM_PURPLE, TERM_YELLOW, TERM_RED}},
{"^ {1,2}(neighbor) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}) (description) ([a-zA-Z0-9 _-]+).$",
					{TERM_YELLOW, TERM_PURPLE, TERM_YELLOW, TERM_BLUE}},



/* Routing */
{"^(Routing entry for )([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})/([0-9]{2}).$", {TERM_YELLOW,TERM_PURPLE,TERM_RED}},
{"^  Known via \"([a-z0-9 ]+)\"", {TERM_CYAN}},

{"^(ip route vrf) ([a-zA-Z0-9_-]*) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3} [0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})(.*)$", 
			{TERM_YELLOW, TERM_RED, TERM_PURPLE, TERM_BRIGHT_GREEN}}, 
{"^(ip route) ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3} [0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})(.*)$", 
{TERM_YELLOW, TERM_PURPLE, TERM_BRIGHT_GREEN}}, 

/* Juniper Hostname  */
{"^(\\{[a-zA-Z0-9_:-]{1,30}\\}).$", {TERM_CYAN}},
{"^(\\{[a-zA-Z0-9_:-]{1,30}\\})(\\[[a-zA-Z0-9_ -]*\\])", {TERM_CYAN, TERM_PURPLE}},
{"^(\\[[a-zA-Z0-9_ -/]*\\]).$", {TERM_PURPLE}},
{"^([a-zA-Z0-9_-]{1,30})@([a-zA-Z0-9\\._-]{1,30})[#>]",{TERM_YELLOW, TERM_BRIGHT_GREEN}},

/* Juniper show | compare */
{"^\\+ (.*)$", {TERM_GREEN}},
{"^\\- (.*)$", {TERM_RED}},

/* Juniper config */
{"^[ ]*([-a-zA-Z0-9_/]+) ([^ ]+) (\\{).$", {TERM_BRIGHT_YELLOW, TERM_PURPLE, TERM_RED}},
{"^[ ]*([a-zA-Z0-9_ -/]+) (\\{).$", {TERM_BRIGHT_YELLOW, TERM_RED}},
{"^[ ]*(}).$", {TERM_RED}},
{"^[ ]*([a-zA-Z0-9/\\._-]+) ([^ ]+);.$", {TERM_LIGHT_BLUE, TERM_CYAN}},
{"^[ ]*([a-zA-Z0-9/\\._-]+);.$", {TERM_CYAN}},
{"^[ ]*([a-zA-Z0-9/\\._-]+) (\\[) (.+) (\\]);.$", {TERM_LIGHT_BLUE, TERM_YELLOW, TERM_CYAN, TERM_YELLOW}},

/* Big numbers */
{" ([0-9]{1,3})([0-9]{3})([0-9]{3})([0-9]{3}) ", {TERM_PURPLE, TERM_CYAN, TERM_BLUE, TERM_CYAN}},
{" ([0-9]{1,3})([0-9]{3})([0-9]{3}) ", {TERM_CYAN, TERM_BLUE, TERM_CYAN}}

	};

/*	This function loops through each regex and compiles it so it's ready to use  */
int init_patterns()
{



	unsigned int i;
        /* compile regexs */

        for (i = 0; i < sizeof(patterns)/sizeof(patterns[0]); i++)
        {
                if (regcomp(&patterns[i].compiled, patterns[i].pattern, REG_EXTENDED|REG_NEWLINE))
                {
                        fprintf(stderr, "Error compiling pattern: %s\n",
                                patterns[i].pattern);

                        return 1;
                }
        }	
	return 0;
}


/* This function is called for every incoming packet. It compares it to each regex and inserts 
   codes as appropriate */
int process_patterns(char *data, int data_len)
{
        unsigned int i; /* main for loop, keeps track of which rule/pattern we're on */
        int c; /* Used in inner loop to keep track of which sub-expression we're at */
        char buffer[32767]; /* Buffer, we rebuild strings with color codes here */
	char mydata[32767]; /* Keeps track of the data between loops */
	int increased;  /* Keeps track of how much we lengthened data */
	regmatch_t matches[MAXMATCH]; /* Used as results of regexec for what we matched */
	regoff_t last_match; /* used to loop through the data incase we match the data more than once */

	increased = 0;

	/* Copy the string to the correct length. Data refrences the whole SSH buffer (I think) and has garbage */

	memset(mydata, '\0', sizeof(mydata));
	strncat(mydata, data, data_len);

        /* execute regexs */
        for (i = 0; i < sizeof(patterns)/sizeof(patterns[0]); i++)
        {
		last_match = 0;
                while (regexec(&patterns[i].compiled, mydata + last_match, MAXMATCH, matches, 0) == 0)
                {
                        if ((int)matches[1].rm_eo > 0)
                        {

				
                                /* Reset variables */
                                memset(buffer, '\0', sizeof(buffer));
				c = 1;

                                /* Copy start of string */
                                strncat(buffer, mydata, (int)matches[c].rm_so + last_match);

				while (((int)matches[c].rm_so >= 0) && (c < MAXMATCH) )
				{
                                	/* Insert before match */
                                	strcat(buffer, patterns[i].insert_before[c-1]);

                                	/*copy what matched */
                                	strncat(buffer, mydata + (int)matches[c].rm_so + last_match, 
							(int)matches[c].rm_eo - (int)matches[c].rm_so);

                                	/* Insert the after match */
                               		 strcat(buffer, TERM_NONE);
	
					/* Are there any more matches for this pattern? */
					if ( (c+1 > MAXMATCH) || ((int)matches[c+1].rm_so < 0 ))
					{
                                		/* Nope! Finish string! */
                                		strncat(buffer, mydata + (int)matches[c].rm_eo + last_match, 
							strlen(mydata) - (int)matches[c].rm_eo + last_match);
					} else
					{
						/* Copy up until next match */
						strncat(buffer, mydata + (int)matches[c].rm_eo + last_match,
							(int)matches[c+1].rm_so - (int)matches[c].rm_eo);
					}
					
					/* Increase the data length by what we added */
					increased += strlen(patterns[i].insert_before[c-1]) + strlen(TERM_NONE);
					c++;
				}
				/* Copy the buffer back to mydata */
				strcpy(mydata, buffer);
			}
		
			last_match += (int)matches[0].rm_eo;
		}
	}

	/* Copy my data back to real data, and return increased length (used in channels.c) */
	strcpy(data, mydata);
	return increased;
}












