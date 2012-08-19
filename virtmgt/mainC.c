/*
 * =====================================================================================
 *
 *       Filename:  knock.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/13/2012 10:42:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <argp.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <sys/stat.h>

#define PORT 3001
#define MAXDATASIZE 5000

#define create_port 3500
#define createall_port 3501
#define list_port 3502
#define getstate_port 3503
#define listnode_port 3504
#define reboot_port 3505


int is_netflowall = 0;
int number;
const char * ip = NULL;

static int timer; 
static struct timeval startTime;
static struct timeval endTime;
static int flag = 1;

char *node_ip[2] = {"10.5.0.121", "10.5.0.227"};

void get_network_flow(char buf[10240], long long * rx_bytes, long long * tx_bytes)
{
	char *s1;
	char *s2;
	s1 = strstr(buf, "rx_bytes");
	s1 += 10;
	for(s2 = s1; *s2 != ','; s2 ++)
		;
	*s2 = '\0';
	*rx_bytes = atoll(s1);
	*s2 = ',';
	s1 = strstr(buf, "tx_bytes");
	s1 += 10;
	for(s2 = s1; *s2 != '\n'; s2 ++)
		;
	*s2 = '\0';
	*tx_bytes = atoll(s1);
}


void cb_quit() 
{ 
	is_netflowall = 0;
	gtk_timeout_remove (timer); 
	gtk_main_quit(); 
}


int cal_load1(double * rflow, double * tflow, double * rflow_total, double * tflow_total) 
{ 	
	long long  rx_bytes = 0;
	long long  tx_bytes = 0;
	int  sockfd, nbytes;
	char buff[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buff, "netflowall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("netflow socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}

	if(write(sockfd, buff, strlen(buff)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buff, MAXDATASIZE)) == -1) {
			perror("read error");
			exit(-1);
	}
	buff[nbytes] = '\0';
	if(!(strstr(buff, "rx_bytes"))) {
		cb_quit();
		printf("%s", buff);
		return;
	}
	//printf("%s", buf);
	get_network_flow(buff, &rx_bytes, &tx_bytes);
	printf("rx_bytes = %lld, tx_bytes = %lld\n", rx_bytes, tx_bytes);
	//printf("hoho\n");
	close(sockfd);

	static long pre_rtraffic, pre_ttraffic;
	long rtraffic, ttraffic;
	long d_rtraffic, d_ttraffic;
	/*int Time;
	if (gettimeofday(&endTime, NULL) == -1) {
   	printf("Failed to get start time\n");
	}
	Time = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
	startTime = endTime;
	*/
	//calculate r netflow  
	rtraffic = rx_bytes;
	d_rtraffic  = rtraffic - pre_rtraffic;
	pre_rtraffic = rtraffic;
	*rflow = d_rtraffic/(1 * 1024);
	*rflow_total = rtraffic / 1024;

	//calculate t netflow  
	ttraffic = tx_bytes;
	d_ttraffic  = ttraffic - pre_ttraffic;
	pre_ttraffic = ttraffic;
	*tflow = d_ttraffic/(1 * 1024);
	*tflow_total = ttraffic / 1024;	

	return 1;
}

int cal_load (double * rflow, double * tflow, double * rflow_total, double * tflow_total) 
{ 	
	long long  rx_bytes, tx_bytes;
	int  sockfd, nbytes;
	char buff[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	//printf("netflow: %d\n", number);
	sprintf(buff, "netflow %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("netflow socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}

	if(write(sockfd, buff, strlen(buff)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buff, MAXDATASIZE)) == -1) {
			perror("read error");
			exit(-1);
	}
	buff[nbytes] = '\0';
	if(!(strstr(buff, "rx_bytes"))) {
		cb_quit();
		printf("%s", buff);
		return;
	}
	//printf("%s", buf);
	get_network_flow(buff, &rx_bytes, &tx_bytes);
	printf("rx_bytes = %lld, tx_bytes = %lld\n", rx_bytes, tx_bytes);
	close(sockfd);

	static long pre_rtraffic, pre_ttraffic;
	long rtraffic, ttraffic;
	long d_rtraffic, d_ttraffic;
	/*int Time;
	if (gettimeofday(&endTime, NULL) == -1) {
   	printf("Failed to get start time\n");
	}
	Time = 1000000 * (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
	startTime = endTime;
	*/
	//calculate r netflow  
	rtraffic = rx_bytes;
	d_rtraffic  = rtraffic - pre_rtraffic;
	pre_rtraffic = rtraffic;
	*rflow = d_rtraffic/(1 * 1024);
	*rflow_total = rtraffic / 1024;

	//calculate t netflow  
	ttraffic = tx_bytes;
	d_ttraffic  = ttraffic - pre_ttraffic;
	pre_ttraffic = ttraffic;
	*tflow = d_ttraffic/(1 * 1024);
	*tflow_total = ttraffic / 1024;	

	return 1;
}



gint Repaint (gpointer da) 
{ 
	GtkWidget * drawing_area = (GtkWidget *)da; 
	GdkDrawable * drawable = drawing_area->window;
	GdkDrawable * tdrawable = drawing_area->window; 

	GdkGC *gc_rchart, *gc_tchart, *gc_rtext, *gc_ttext; 
	GdkColor color; 

	static unsigned rcoory[401]; 
	static unsigned tcoory[401]; 
	int da_width, da_height; 
	int x; 
	//double load; 
	char buf[30]; 
	double rflow, tflow, rflow_total, tflow_total;

	//green rchart 
	gc_rchart = gdk_gc_new (drawing_area->window); 
	color.red = 0; 
	color.green = 0xffff; 
	color.blue = 0; 
	gdk_gc_set_rgb_fg_color(gc_rchart, &color ); 
	//green tchart 
	gc_tchart = gdk_gc_new (drawing_area->window); 
	color.red = 0; 
	color.green = 0xffff; 
	color.blue = 0; 
	gdk_gc_set_rgb_fg_color(gc_tchart, &color );
	//red rtext
	gc_rtext = gdk_gc_new (drawing_area->window); 
	color.red = 0xffff; 
	color.green = 0; 
	color.blue = 0; 
	gdk_gc_set_rgb_fg_color(gc_rtext, &color ); 
	//red ttext
	gc_ttext = gdk_gc_new (drawing_area->window); 
	color.red = 0xffff; 
	color.green = 0; 
	color.blue = 0; 
	gdk_gc_set_rgb_fg_color(gc_ttext, &color ); 

	// size 
	da_width = drawing_area->allocation.width; 
	da_height = drawing_area->allocation.height; 
	
	// recieve & t
	gdk_draw_rectangle (drawable, drawing_area->style->white_gc, TRUE, 0, 0, da_width, 0.5 * da_height); 
	gdk_draw_rectangle (tdrawable, drawing_area->style->white_gc, TRUE, 0, 201, da_width, 0.5 * da_height);
	// chart line
	for (x=0; x<400; x++) 
		rcoory[x] = rcoory[x+1]; 
	for (x=0; x<400; x++) 
		tcoory[x] = tcoory[x+1]; 
	if(is_netflowall) {
		cal_load1(&rflow, &tflow, &rflow_total, &tflow_total);
	} else
		cal_load(&rflow, &tflow, &rflow_total, &tflow_total);
	rcoory[x] = (int)(rflow * 0.01 * 0.5 * da_height); 
	tcoory[x] = (int)(tflow * 0.01 * 0.5 * da_height); 
	
	if(flag){
		rcoory[x] = 0; 
		tcoory[x] = 0;
		flag= 0;
	//printf("hello!\n");
	} 
	//printf("rcoory = %d, tcoory = %d\n", rcoory[x], tcoory[x]);
	for(x=0;x<da_width;x++){ 
		gdk_draw_line (drawable, gc_rchart, x, 0.5 * da_height, x, 0.5 * da_height - rcoory[x]); 
	} 
	for(x=0;x<da_width;x++){ 
		gdk_draw_line (tdrawable, gc_tchart, x, da_height, x, da_height - tcoory[x]); 
	} 

	// following code for drawing rtext 
	sprintf (buf, "R Flow:%.1f kb/s", rflow ); 

	PangoLayout *layout = gtk_widget_create_pango_layout( da, buf );
	//set font
	PangoFontDescription *fontdesc = pango_font_description_from_string( "Luxi Mono 12" ); 
	pango_layout_set_font_description( layout, fontdesc ); 
	gdk_draw_layout( drawable, gc_rtext, 5, 5, layout ); 
	pango_font_description_free( fontdesc ); 
	g_object_unref( layout ); 

	sprintf (buf, "R Total flow:%.1f kb", rflow_total ); 
	PangoLayout *layout1 = gtk_widget_create_pango_layout( da, buf );
	//set font
	PangoFontDescription *fontdesc1 = pango_font_description_from_string( "Luxi Mono 12" ); 
	pango_layout_set_font_description( layout1, fontdesc1 ); 
	gdk_draw_layout( drawable, gc_rtext, 180, 5, layout1 ); 
	pango_font_description_free( fontdesc1 ); 
	g_object_unref( layout1 ); 

	// following code for drawing ttext 
	sprintf (buf, "T Flow:%.1f kb/s", tflow ); 

	PangoLayout *layout2 = gtk_widget_create_pango_layout( da, buf );
	//set font
	PangoFontDescription *fontdesc2 = pango_font_description_from_string( "Luxi Mono 12" ); 
	pango_layout_set_font_description( layout2, fontdesc2 ); 
	gdk_draw_layout( drawable, gc_ttext, 5, 205, layout2 ); 
	pango_font_description_free( fontdesc2 ); 
	g_object_unref( layout2 ); 

	sprintf (buf, "R Total flow:%.1f kb", tflow_total ); 
	PangoLayout *layout3 = gtk_widget_create_pango_layout( da, buf );
	//set font
	PangoFontDescription *fontdesc3 = pango_font_description_from_string( "Luxi Mono 12" ); 
	pango_layout_set_font_description( layout3, fontdesc3 ); 
	gdk_draw_layout( drawable, gc_ttext, 180, 205, layout3 ); 
	pango_font_description_free( fontdesc3 ); 
	g_object_unref( layout3 ); 

	/*// t
	//	gdk_draw_rectangle (tdrawable, drawing_area->style->white_gc, TRUE, 0, 201, da_width, 0.5 * da_height); 

	// chart line
	for (x=0; x<400; x++) 
		tcoory[x] = tcoory[x+1]; 
 
	tcoory[x] = (int)(load * 0.05 * da_height); 

	for(x=0;x<da_width;x++){ 
		gdk_draw_line (tdrawable, gc_tchart, x, da_height, x, da_height - tcoory[x]); 
	} 
	// following code for drawing ttext 
	sprintf (buf, "T Flow:%.1f kb/s", load ); 

	PangoLayout *layout2 = gtk_widget_create_pango_layout( da, buf );
	//set font
	PangoFontDescription *fontdesc2 = pango_font_description_from_string( "Luxi Mono 12" ); 
	pango_layout_set_font_description( layout2, fontdesc2 ); 
	gdk_draw_layout( drawable, gc_ttext, 5, 205, layout2 ); 
	pango_font_description_free( fontdesc2 ); 
	g_object_unref( layout2 ); 

	sprintf (buf, "R Total flow:%.1f kb", testnum ); 
	PangoLayout *layout3 = gtk_widget_create_pango_layout( da, buf );
	//set font
	PangoFontDescription *fontdesc3 = pango_font_description_from_string( "Luxi Mono 12" ); 
	pango_layout_set_font_description( layout3, fontdesc3 ); 
	gdk_draw_layout( drawable, gc_ttext, 180, 205, layout3 ); 
	pango_font_description_free( fontdesc3 ); 
	g_object_unref( layout3 ); */

	g_object_unref( G_OBJECT(gc_rchart) ); 
	g_object_unref( G_OBJECT(gc_tchart) );
	g_object_unref( G_OBJECT(gc_rtext) ); 
	g_object_unref( G_OBJECT(gc_ttext) ); 

	return TRUE; 
}

/* *******************  undefinevmall  ******************** */
static char undefinevmall_doc[] = 
	"undefinevmall: undefine all the vm xml of a node\n"
	"--ip must be provided.";

static char undefinevmall_args_doc[] = 
	"";

static struct argp_option undefinevmall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_undefinevmall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp undefinevmall_argp = {undefinevmall_options, parse_undefinevmall_opt, undefinevmall_args_doc, undefinevmall_doc, NULL, NULL, NULL};

void undefinevmall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&undefinevmall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "undefinevmall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("undefinevmall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}



/* ************  undefinevm  ***************/
static char undefinevm_doc[] = 
	"undefinevm: undefine new virtual machine\n"
	"--number, --ip must be provided.";

static char undefinevm_args_doc[] = 
	"";

static struct argp_option undefinevm_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_undefinevm_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp undefinevm_argp = {undefinevm_options, parse_undefinevm_opt, undefinevm_args_doc, undefinevm_doc, NULL, NULL, NULL};

void undefinevm(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&undefinevm_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}
	
	sprintf(buf, "undefinevm %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("undefinevm socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}


/* *******************  definevmall  ******************** */

static char definevmall_doc[] = 
	"definevmall: define all the vm xml of a node\n"
	"--ip must be provided.";

static char definevmall_args_doc[] = 
	"";

static struct argp_option definevmall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_definevmall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp definevmall_argp = {definevmall_options, parse_definevmall_opt, definevmall_args_doc, definevmall_doc, NULL, NULL, NULL};

void definevmall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&definevmall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "definevmall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("definevmall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}



/* ************  definevm  ***************/
static char definevm_doc[] = 
	"definevm: define new virtual machine\n"
	"--number, --ip must be provided.";

static char definevm_args_doc[] = 
	"";

static struct argp_option definevm_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_definevm_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp definevm_argp = {definevm_options, parse_definevm_opt,
	definevm_args_doc, definevm_doc, NULL, NULL, NULL};

void definevm(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&definevm_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}
	
	sprintf(buf, "definevm %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("definevm socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}

/* *********************  listnode  ********************** */
static char listnode_doc[] = 
	"listnode: list info of node\n"
	"--ip must be provided.";

static char listnode_args_doc[] = 
	"";

static struct argp_option listnode_options[] = {
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_listnode_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp listnode_argp = {listnode_options, parse_listnode_opt,
	listnode_args_doc, listnode_doc, NULL, NULL, NULL};

void listnode(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&listnode_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	sprintf(buf, "listnode");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("listnode socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}

/* *********************  getstate  ********************** */

void getstate(int argc, char ** argv)
{
	int sockfd, nbytes, i;
	char buf[10240] = {0};
	char buf_tmp[10240] = {0};
	char *buf_cmd = "getstate";
	struct hostent *he;
	struct sockaddr_in srvaddr;
	
	for(i = 0; node_ip[i]; i++) {
		//printf("node%d : %s\n", i, node_ip[i]);
	 
		if((he = gethostbyname(node_ip[i])) == NULL) {
			perror("gethostbyname");
			exit(-1);
		}

		if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			perror("getstate socket error");
			exit(-1);
		}
		bzero(&srvaddr, sizeof(srvaddr));

		srvaddr.sin_family = AF_INET;
		srvaddr.sin_port = htons(PORT);
		srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

		if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
			perror("connect error");
			exit(-1);
		}
		if(write(sockfd, buf_cmd, strlen(buf_cmd)) == -1) {
			perror("send error");
			exit(-1);
		}

		if((nbytes = read(sockfd, buf_tmp, MAXDATASIZE)) == -1) {
			perror("read error");
			exit(-1);
		}
		close(sockfd);
		if(strlen(buf_tmp) != 0) {
			sprintf(&buf[strlen(buf)], "node%s%s", node_ip[i], buf_tmp);
		}
	}
	buf[strlen(buf)] = '\n';
	printf("%s", buf);
}




/* *********************  reboot  ********************** */
static char reboot_doc[] = 
	"reboot: reboot virtual machine\n"
	"--number, --ip must be provided.";

static char reboot_args_doc[] = 
	"";

static struct argp_option reboot_options[] = {
	{"ip", 'i', "ip address", 0, "ip address of virtual machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_reboot_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp reboot_argp = {reboot_options, parse_reboot_opt,
	reboot_args_doc, reboot_doc, NULL, NULL, NULL};

void re_boot(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&reboot_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	sprintf(buf, "reboot");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("reboot socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(3002);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}



/* *******************  resumeall  ******************** */

static char resumeall_doc[] = 
	"resumeall: resume all the virtual machines of a node\n"
	"--ip must be provided.";

static char resumeall_args_doc[] = 
	"";

static struct argp_option resumeall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_resumeall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp resumeall_argp = {resumeall_options, parse_resumeall_opt, resumeall_args_doc, resumeall_doc, NULL, NULL, NULL};

void resumeall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&resumeall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "resumeall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("resumeall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}


/* *********************  resume  ********************** */
static char resume_doc[] = 
	"resume: resume virtual machine\n"
	"--number, --ip must be provided.";

static char resume_args_doc[] = 
	"";

static struct argp_option resume_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_resume_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp resume_argp = {resume_options, parse_resume_opt,
	resume_args_doc, resume_doc, NULL, NULL, NULL};

void resume(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&resume_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 
	
	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	sprintf(buf, "resume %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("resume socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}



/* *******************  suspendall  ******************** */
static char suspendall_doc[] = 
	"suspendall: suspend all the virtual machines of a node\n"
	"--ip must be provided.";

static char suspendall_args_doc[] = 
	"";

static struct argp_option suspendall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_suspendall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp suspendall_argp = {suspendall_options, parse_suspendall_opt, suspendall_args_doc, suspendall_doc, NULL, NULL, NULL};



void suspendall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&suspendall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "suspendall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("suspendall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}


/* *********************  suspend  ********************** */
static char suspend_doc[] = 
	"suspend: suspend virtual machine\n"
	"--number, --ip must be provided.";

static char suspend_args_doc[] = 
	"";

static struct argp_option suspend_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_suspend_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp suspend_argp = {suspend_options, parse_suspend_opt,
	suspend_args_doc, suspend_doc, NULL, NULL, NULL};

void suspend(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&suspend_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	sprintf(buf, "suspend %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("suspend socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}



/* *******************  shutdownall  ******************** */
static char shutdownall_doc[] = 
	"shutdownall: shutdown all the virtual machines of a node\n"
	"--ip must be provided.";

static char shutdownall_args_doc[] = 
	"";

static struct argp_option shutdownall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_shutdownall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp shutdownall_argp = {shutdownall_options, parse_shutdownall_opt, shutdownall_args_doc, shutdownall_doc, NULL, NULL, NULL};


void shutdownall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&shutdownall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "shutdownall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("shutdownall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}


/* *********************  shutdown  ********************** */
static char shutdown_doc[] = 
	"shutdown: shutdown virtual machine\n"
	"--number, --ip must be provided.";

static char shutdown_args_doc[] = 
	"";

static struct argp_option shutdown_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_shutdown_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp shutdown_argp = {shutdown_options, parse_shutdown_opt,
	shutdown_args_doc, shutdown_doc, NULL, NULL, NULL};

void shut_down(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&shutdown_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 
	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	sprintf(buf, "shutdown %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("shutdown socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}


/* *******************  destroyall  ******************** */
static char destroyall_doc[] = 
	"destroyall: destroy all the virtual machines of a node\n"
	"--ip must be provided.";

static char destroyall_args_doc[] = 
	"";

static struct argp_option destroyall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_destroyall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp destroyall_argp = {destroyall_options, parse_destroyall_opt, destroyall_args_doc, destroyall_doc, NULL, NULL, NULL};


void destroyall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&destroyall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "destroyall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("destroyall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}


/* *********************  destroy  ********************** */
static char destroy_doc[] = 
	"destroy: destroy virtual machine\n"
	"--number, --ip must be provided.";

static char destroy_args_doc[] = 
	"";

static struct argp_option destroy_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_destroy_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp destroy_argp = {destroy_options, parse_destroy_opt,
	destroy_args_doc, destroy_doc, NULL, NULL, NULL};

void destroy(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&destroy_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	sprintf(buf, "destroy %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("destroy socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}

/* *******************  netflowall  ******************** */

static char netflowall_doc[] = 
	"netflowall: display the total netflow of a node\n"
	"--ip must be provided.";

static char netflowall_args_doc[] = 
	"";

static struct argp_option netflowall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_netflowall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp netflowall_argp = {netflowall_options, parse_netflowall_opt, netflowall_args_doc, netflowall_doc, NULL, NULL, NULL};



int netflowall(int argc, char ** argv) 
{
	int idx;
	int err = argp_parse(&netflowall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	is_netflowall = 1;
	printf("is_netflowall = %d\n", is_netflowall);
	GtkWidget *window; 
	GtkWidget *drawing_area; 

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 
	gtk_window_set_title (GTK_WINDOW(window), "Network traffic"); 
	drawing_area = gtk_drawing_area_new (); 
	gtk_container_add (GTK_CONTAINER(window), drawing_area); 

	gtk_signal_connect(GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC(cb_quit), NULL); 

	g_signal_connect( drawing_area, "expose_event", G_CALLBACK(Repaint), NULL ); 
 
	gtk_drawing_area_size (GTK_DRAWING_AREA(drawing_area), 400, 400); 

	gtk_widget_show (drawing_area); 
	gtk_widget_show(window); 
	if (gettimeofday(&startTime, NULL) == -1) {
   	printf("Failed to get start time\n");
		return -1;
	}
	timer = gtk_timeout_add (1000, Repaint, (gpointer) drawing_area); 
	gtk_main(); 

	return 0; 
}




/* *******************  netflow  ******************** */
 

static char netflow_doc[] = 
	"netflow: display the netflow of virtual machine\n"
	"--number, --ip must be provided.";

static char netflow_args_doc[] = 
	"";

static struct argp_option netflow_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_netflow_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp netflow_argp = {netflow_options, parse_netflow_opt,
	netflow_args_doc, netflow_doc, NULL, NULL, NULL};

int netflow(int argc, char ** argv) 
{ 
	int idx;
	int err = argp_parse(&netflow_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	}

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	GtkWidget *window; 
	GtkWidget *drawing_area; 

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL); 
	gtk_window_set_title (GTK_WINDOW(window), "Network traffic"); 
	drawing_area = gtk_drawing_area_new (); 
	gtk_container_add (GTK_CONTAINER(window), drawing_area); 

	gtk_signal_connect(GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC(cb_quit), NULL); 

	g_signal_connect( drawing_area, "expose_event", G_CALLBACK(Repaint), NULL ); 
 
	gtk_drawing_area_size (GTK_DRAWING_AREA(drawing_area), 400, 400); 

	gtk_widget_show (drawing_area); 
	gtk_widget_show(window); 
	if (gettimeofday(&startTime, NULL) == -1) {
   	printf("Failed to get start time\n");
		return -1;
	}
	timer = gtk_timeout_add (1000, Repaint, (gpointer) drawing_area); 
	gtk_main(); 

	return 0; 
}


/* *******************  listall  ******************** */
static char listall_doc[] = 
	"listall: list the info of all virtual machines on a node\n"
	"--ip must be provided.";

static char listall_args_doc[] = 
	"";

static struct argp_option listall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_listall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp listall_argp = {listall_options, parse_listall_opt, listall_args_doc, listall_doc, NULL, NULL, NULL};



void listall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&listall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;
	sprintf(buf, "listall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("listall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}


/* *********************  list  ********************** */
static char list_doc[] = 
	"list: list info of virtual machine\n"
	"--number, --ip must be provided.";

static char list_args_doc[] = 
	"";

static struct argp_option list_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_list_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp list_argp = {list_options, parse_list_opt,
	list_args_doc, list_doc, NULL, NULL, NULL};

void list(int argc, char ** argv)
{

	long long  rx_bytes, tx_bytes;
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&list_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}


	sprintf(buf, "list %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("list socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s", buf);
	close(sockfd);
}

/* *******************  createall  ******************** */

static char createall_doc[] = 
	"createall: create all the virtual machines of a node\n"
	"--ip must be provided.";

static char createall_args_doc[] = 
	"";

static struct argp_option createall_options[] = {
	{"ip", 'i', "ip address", 0 ,"ip address of node machine", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_createall_opt(int key, char * arg, 
		struct argp_state * state )
{
	switch (key) {
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp createall_argp = {createall_options, parse_createall_opt, createall_args_doc, createall_doc, NULL, NULL, NULL};

void createall(int argc, char ** argv)
{
	int idx;
	int err = argp_parse(&createall_argp, argc, argv,
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	
	if (ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}

	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	sprintf(buf, "createall");
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("createall socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}



/* ************  create  ***************/
static char create_doc[] = 
	"create: create new virtual machine\n"
	"--number, --ip must be provided.";

static char create_args_doc[] = 
	"";

static struct argp_option create_options[] = {
	{"number", 'n', "target number", 0 ,"number of virtual machine", 0},
	{"ip", 'i', "ip address", 0, "ip address of node", 0},
	{NULL, '\0', NULL, 0, NULL ,0},
};

static error_t 
parse_create_opt(int key, char * arg, 
		struct argp_state * state /*__attribute__((unused))*/ )
{
	switch (key) {
	case 'n' :
		number = atoi(arg);
		return 0;
	case 'i' :
		ip = arg;
		return 0;
	default:
		return ARGP_ERR_UNKNOWN;
	}
}

static struct argp create_argp = {create_options, parse_create_opt,
	create_args_doc, create_doc, NULL, NULL, NULL};

void create(int argc, char ** argv)
{
	int sockfd, nbytes;
	char buf[10240] = {0};
	struct hostent *he;
	struct sockaddr_in srvaddr;

	int idx;
	int err = argp_parse(&create_argp, argc, argv, 
			ARGP_IN_ORDER, &idx, NULL);
	if (err != 0) {
		printf("argp_parse error: %d\n", err);
		exit(-1);
	}
	if(number < 1 || number > 100) {
		printf("n is between 1 and 100\n");
		exit(-1);
	} 

	if(ip == NULL) {
		printf("use -i or --ip to set ip address\n");
		exit(-1);
	}
	
	sprintf(buf, "create %d", number);
  
	if((he = gethostbyname(ip)) == NULL) {
		perror("gethostbyname");
		exit(-1);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("create socket error");
		exit(-1);
	}
	bzero(&srvaddr, sizeof(srvaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr = *((struct in_addr *)he->h_addr);

	if(connect(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(-1);
	}
	if(write(sockfd, buf, strlen(buf)) == -1) {
		perror("send error");
		exit(-1);
	}

	if((nbytes = read(sockfd, buf, MAXDATASIZE)) == -1) {
		perror("read error");
		exit(-1);
	}

	buf[nbytes] = '\0';
	printf("%s\n", buf);
	close(sockfd);
}

/* ***************  usage  ***************** */
static void usage(const char * arg)
{
	printf("usage:\n");
	printf("\t%s create -n (1--100) -i node_ip\n", arg);
	printf("\t%s list -n (1--100) -i node_ip\n", arg);
	printf("\t%s netflow -n (1--100) -i node_ip\n", arg);
	printf("\t%s destroy -n (1--100) -i node_ip\n", arg);
	printf("\t%s shutdown -n (1--100) -i node_ip\n", arg);
	printf("\t%s suspend -n (1--100) -i node_ip\n", arg);
	printf("\t%s resume -n (1--100) -i node_ip\n", arg);
	printf("\t%s definevm -n (1--100) -i node_ip\n", arg);
	printf("\t%s undefinevm -n (1--100) -i node_ip\n", arg);
	printf("\t%s createall -i node_ip\n", arg);
	printf("\t%s listall -i node_ip\n", arg);
	printf("\t%s netflowall -i node_ip\n", arg);
	printf("\t%s destroyall -i node_ip\n", arg);
	printf("\t%s shutdownall -i node_ip\n", arg);
	printf("\t%s suspendall -i node_ip\n", arg);
	printf("\t%s resumeall -i node_ip\n", arg);
	printf("\t%s definevmall -i node_ip\n", arg);
	printf("\t%s undefinevmall -i node_ip\n", arg);
	printf("\t%s reboot -i vm_ip\n", arg);
	printf("\t%s getstate\n", arg);
	printf("\t%s listnode -i node_ip\n", arg);

	exit(-1);
}

/* **********************  main  ****************** */
int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	if(argc < 2)
		usage(argv[0]);
	if(strcmp(argv[1], "create") == 0) {
		create(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "createall") == 0) {
		createall(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "list") == 0) {
		list(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "listall") == 0) {
		listall(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "netflow") == 0) {
		netflow(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "netflowall") == 0) {
		netflowall(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "destroyall") == 0) {
		destroyall(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "destroy") == 0) {
		destroy(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "shutdownall") == 0) {
		shutdownall(argc - 1, &argv[1]);
	} else if (strcmp(argv[1], "shutdown") == 0) {
		shut_down(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "suspendall") == 0) {
		suspendall(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "suspend") == 0) {
		suspend(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "resumeall") == 0) {
		resumeall(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "resume") == 0) {
		resume(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "reboot") == 0) {
		re_boot(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "getstate") == 0) {
		getstate(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "listnode") == 0) {
		listnode(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "definevmall") == 0) {
		definevmall(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "definevm") == 0) {
		definevm(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "undefinevmall") == 0) {
		undefinevmall(argc - 1, &argv[1]);
	} else if(strcmp(argv[1], "undefinevm") == 0) {
		undefinevm(argc - 1, &argv[1]);
	}
	else
		usage(argv[0]);
	return 0;
}
