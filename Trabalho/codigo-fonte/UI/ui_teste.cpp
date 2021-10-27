#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
//#include <gtk/gtkx.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include <pthread.h>
#include <cstddef>


//window
GtkWidget  *window;

//containers
GtkWidget  *main_frame;
GtkWidget  *first_btns;
GtkWidget  *sec_btns;
GtkWidget  *dc_win;
GtkWidget  *dc_viewp;
GtkWidget  *dc_grid;
GtkWidget  *text_entry;
GtkWidget  *method_selector;



//buttons
GtkWidget  *m_decks;
GtkWidget  *m_cards;
GtkWidget  *add_btn;
GtkWidget  *rem_btn;
GtkWidget  *exit_btn;
GtkWidget  *back_btn;
GtkWidget  *enter_btn;
GtkWidget  *camera_btn;
GtkWidget  *manual_btn;
GtkWidget  *back_btn2;

//text entry
GtkWidget  *get_usr_entry;

//rows of cards and decks
GtkWidget *label[1000];
GtkWidget *button[1000];
char actual_deck[1024];


//builder
GtkBuilder *builder;

//aux
char tmp[1024];
char tmp2[1024];
int row = 0;
int cd_flag = 0;
int rem_flag = 0;
int flag_card = -1;

void on_m_decks_clicked(GtkButton *);
void on_m_cards_clicked(GtkButton *);
void on_add_btn_clicked(GtkButton *);
void on_rem_btn_clicked(GtkButton *);
void on_exit_btn_clicked (GtkButton *);
void on_back_btn_clicked (GtkButton *);
void on_enter_btn_clicked (GtkButton *);
void on_camera_btn_clicked (GtkButton *);
void on_manual_btn_clicked (GtkButton *);
void on_back_btn2_clicked (GtkButton *);
void on_row(GtkButton *);
void on_row2(GtkButton *);
void delete_rows();
void update_list();
int comparech(const char* str1,const char* str2);


//ocr
using namespace std;
using namespace cv;
char *nome2;
char tesse();
int quadro();
int cards();
int tesste(char *name);


int main(int argc, char *argv[])
{	system("export LC_ALL=C");
	gtk_init(&argc, &argv);
	
	
	builder = gtk_builder_new_from_file("Ui_teste.glade");
	//load windows
	window = GTK_WIDGET(gtk_builder_get_object(builder,"gtk_window"));
	text_entry = GTK_WIDGET(gtk_builder_get_object(builder,"text_entry"));
	method_selector = GTK_WIDGET(gtk_builder_get_object(builder,"method_selector"));
	
	g_signal_connect(window,"destroy", G_CALLBACK(gtk_main_quit),NULL);
	
	
	gtk_builder_connect_signals(builder,NULL);
	
	//load buttons
	m_decks = GTK_WIDGET(gtk_builder_get_object(builder,"m_decks"));
	m_cards = GTK_WIDGET(gtk_builder_get_object(builder,"m_cards"));
	add_btn = GTK_WIDGET(gtk_builder_get_object(builder,"add_btn"));
	rem_btn = GTK_WIDGET(gtk_builder_get_object(builder,"rem_btn"));
	back_btn = GTK_WIDGET(gtk_builder_get_object(builder,"back_btn"));
	enter_btn = GTK_WIDGET(gtk_builder_get_object(builder,"enter_btn"));
	camera_btn = GTK_WIDGET(gtk_builder_get_object(builder,"camera_btn"));
	manual_btn = GTK_WIDGET(gtk_builder_get_object(builder,"manual_btn"));
	back_btn2 = GTK_WIDGET(gtk_builder_get_object(builder,"back_btn2"));
	
	//usr entry
	get_usr_entry = GTK_WIDGET(gtk_builder_get_object(builder,"get_usr_entry"));
	
	//load list
	dc_win = GTK_WIDGET(gtk_builder_get_object(builder,"dc_win"));
	dc_viewp = GTK_WIDGET(gtk_builder_get_object(builder,"dc_viewp"));
	dc_grid = GTK_WIDGET(gtk_builder_get_object(builder,"dc_grid"));
	
	
	//connect signals
	g_signal_connect(m_decks,"clicked",G_CALLBACK(on_m_decks_clicked),NULL);
	g_signal_connect(m_cards,"clicked",G_CALLBACK(on_m_cards_clicked),NULL);
	g_signal_connect(add_btn,"clicked",G_CALLBACK(on_add_btn_clicked),NULL);
	g_signal_connect(rem_btn,"clicked",G_CALLBACK(on_rem_btn_clicked),NULL);
	g_signal_connect(exit_btn,"clicked", G_CALLBACK(gtk_widget_destroy), window);
	g_signal_connect(back_btn,"clicked",G_CALLBACK(on_back_btn_clicked),NULL);
	g_signal_connect(enter_btn,"clicked",G_CALLBACK(on_enter_btn_clicked),NULL);
	g_signal_connect(back_btn2,"clicked",G_CALLBACK(on_back_btn2_clicked),NULL);
	g_signal_connect(manual_btn,"clicked",G_CALLBACK(on_manual_btn_clicked),NULL);
	g_signal_connect(camera_btn,"clicked",G_CALLBACK(on_camera_btn_clicked),NULL);
	
	//start window
	
	//mudar cor do rem_btn
	GdkColor color;
	gdk_color_parse ("blue", &color);
	gtk_widget_modify_bg ( rem_btn, GTK_STATE_NORMAL, &color);
	
	gtk_widget_show(window);
	gtk_widget_hide(text_entry);
	gtk_widget_hide(method_selector);
	
	gtk_main();
	
	
	return 0;
}

void on_m_decks_clicked(GtkButton *b)
{
	cd_flag = 1;
	
	update_list();
	
}

void on_m_cards_clicked(GtkButton *b)
{
	cd_flag = 3;
	sprintf(actual_deck, "%s", "AllCards"); 
	update_list();
	
}
void on_add_btn_clicked(GtkButton *b)
{
	if(cd_flag == 1 || cd_flag == 3)
	{
		gtk_widget_show_all(text_entry);
	}
	if(cd_flag == 2)
	{
		gtk_widget_show_all(method_selector);
	}
	
}
void on_rem_btn_clicked(GtkButton *b)
{
	if(rem_flag == 1)
	{
		GdkColor color;

		gdk_color_parse ("blue", &color);

		gtk_widget_modify_bg ( GTK_WIDGET(b), GTK_STATE_NORMAL, &color);
		rem_flag=0;
	}
	else if(rem_flag == 0)
	{
		GdkColor color;

		gdk_color_parse ("red", &color);

		gtk_widget_modify_bg ( GTK_WIDGET(b), GTK_STATE_NORMAL, &color);
		rem_flag=1;
	}
	
}


void on_row(GtkButton *b)
{
	//make file string
	char txt[] = ".txt";
	char file_name[1024];
	sprintf(file_name, "%s", gtk_button_get_label(b));  
	strcat(file_name,txt);
	
	if(rem_flag==1)
	{
			char buff[100];
			int n = 0;
			while(1)
			{
				GtkWidget* temp = gtk_grid_get_child_at(GTK_GRID(dc_grid),1, n);
				if(GTK_BUTTON(temp) == b)
					break;
				n++;
				
			}
			n++;
			sprintf(buff,"%d",n);
			
			system("cp Decks.txt aux2.txt");
			char cmd_append[]="sed '";
			strcat(cmd_append,buff);
			strcat(cmd_append,"d' ");
			strcat(cmd_append,"aux2.txt > Decks.txt");
			
			system(cmd_append);
			
			char remove_file[1024] = "rm ";
			sprintf(file_name, "%s", gtk_button_get_label(b));  
			strcat(file_name,".txt");
			strcat(remove_file,file_name);
			
			system(remove_file);
			
			char remove_file2[1024] = "rm q";
			sprintf(file_name, "%s", gtk_button_get_label(b));  
			strcat(file_name,".txt");
			strcat(remove_file2,file_name);
			
			system(remove_file2);
			
			update_list();
			
		
	}
	else
	{
		
		sprintf(actual_deck, "%s", gtk_button_get_label(b)); 
		//reset grid
		update_list();
		cd_flag = 2; 
	}
}

void on_back_btn_clicked (GtkButton *b)
{
	gtk_editable_delete_text(GTK_EDITABLE(get_usr_entry),0,-1);
	gtk_widget_hide(text_entry);
}

void on_enter_btn_clicked (GtkButton *b)
{
	if(cd_flag == 1)
	{
		
		printf("%s\n",gtk_entry_get_text(GTK_ENTRY(get_usr_entry)));
		char txt[] = ".txt";
		char file_name[1024], file_namecp[1024];
		
		sprintf(file_name, "%s", gtk_entry_get_text(GTK_ENTRY(get_usr_entry)));  
		strcpy(file_namecp,file_name);
		strcat(file_name,txt);
		
		FILE *f1 = fopen(file_name, "r");
		if(f1==NULL)
		{
				
			char start_cmd[2048] = "echo ";
			char start_cmd2[2048] = "echo ";
			char end_cmd[] = " >> Decks.txt";
			char end_cmd2[] = "touch ";
			char end_cmd3[] = "touch ";
			
			//adiciona o deck a colecao
			strcat(start_cmd,file_namecp);
			strcat(start_cmd,end_cmd);
			system(start_cmd);
			
			//cria o arquivo do deck
			//strcat(start_cmd2,end_cmd2);
			strcat(end_cmd2,file_name);
			system(end_cmd2);
			
			//cria o arquivo de n de cartas
			strcat(end_cmd3,"q");
			strcat(end_cmd3,file_name);
			system(end_cmd3);
			
			
			update_list();
			
			gtk_editable_delete_text(GTK_EDITABLE(get_usr_entry),0,-1);
			gtk_widget_hide(text_entry);
			return;
		}
		
		
		update_list();
		
		gtk_editable_delete_text(GTK_EDITABLE(get_usr_entry),0,-1);
		gtk_widget_hide(text_entry);
	}
	
	if(cd_flag == 2)
	{
			char txt[] = ".txt";
			char card_name[1024];
			
			char file_name[1024], file_namecp[1024];
			char append_cmd[1024] = "echo ";
			char append_cmd2[1024] = "echo ";
			char actual_deck_q[1024] = "q";
			char actual_deck_aux[1024] = "q";
			char actual_deck_aux2[1024] = "q";
			
			strcpy(actual_deck_aux,actual_deck);
			strcpy(actual_deck_aux2,actual_deck);
			
			
			strcat(actual_deck_aux, txt);
			
			sprintf(card_name, "%s",  gtk_entry_get_text(GTK_ENTRY(get_usr_entry)));  
			int flag_card=tesste(card_name);
			if(flag_card < 0){
				return;
			}
			//add card to deck
			strcat(append_cmd, card_name);
			strcat(append_cmd, ">>");
			strcat(append_cmd, actual_deck_aux);
			system(append_cmd);
			
			strcat(append_cmd2, "1");
			strcat(append_cmd2, " >> ");
			strcat(actual_deck_q,actual_deck_aux2);
			strcat(actual_deck_q,txt);
			
			strcat(append_cmd2, actual_deck_q);
			printf("%s\n",append_cmd2);
			system(append_cmd2);
			
			//make buttons and append to grid
			gtk_editable_delete_text(GTK_EDITABLE(get_usr_entry),0,-1);
			gtk_widget_hide(text_entry);
			update_list();
			
	}
	
	if(cd_flag==3)
	{
			char txt[] = ".txt";
			char card_name[1024];
			
			char file_name[1024], file_namecp[1024];
			char append_cmd[1024] = "echo ";
			char append_cmd2[1024] = "echo ";
			char actual_deck_q[1024] = "q";
			char actual_deck_aux[1024] = "q";
			char actual_deck_aux2[1024] = "q";
			
			strcpy(actual_deck_aux,"AllCards");
			strcpy(actual_deck_aux2,"AllCards");
			
			
			strcat(actual_deck_aux, txt);
			
			sprintf(card_name, "%s",  gtk_entry_get_text(GTK_ENTRY(get_usr_entry)));  
			int flag_card=tesste(card_name);
			if(flag_card < 0){
				return;
			}
			//add card to deck
			strcat(append_cmd, card_name);
			strcat(append_cmd, ">>");
			strcat(append_cmd, actual_deck_aux);
			system(append_cmd);
			
			strcat(append_cmd2, "1");
			strcat(append_cmd2, " >> ");
			strcat(actual_deck_q,actual_deck_aux2);
			strcat(actual_deck_q,txt);
			
			strcat(append_cmd2, actual_deck_q);
			printf("%s\n",append_cmd2);
			system(append_cmd2);
			
			//make buttons and append to grid
			gtk_editable_delete_text(GTK_EDITABLE(get_usr_entry),0,-1);
			gtk_widget_hide(text_entry);
			update_list();
	}
	
	
}

void delete_rows()
{
	GList *children, *iter;
	children = gtk_container_get_children(GTK_CONTAINER(dc_grid));
	for (iter = children; iter!=NULL; iter = g_list_next(iter))
	{
		gtk_widget_destroy(GTK_WIDGET(iter->data));

	}
	g_list_free(children);
}

void update_list()
{
	delete_rows();
	if(cd_flag == 1)
	{
		
		
		
		FILE *f1 = fopen("Decks.txt", "r");
		if(f1==NULL)
		{
			printf("File error!\n");
			exit(1);
		}
		
		//make buttons and append to grid
		row=0;
		while(1)
		{
			if(fgets(tmp,1024,f1)==NULL)
			{
				fclose(f1);
				break;
			}
			tmp[strlen(tmp)-1] = 0;
			
			gtk_grid_insert_row(GTK_GRID(dc_grid), row);

			//add buttons
			button[row] = gtk_button_new_with_label(tmp);
			gtk_button_set_alignment(GTK_BUTTON(button[row]),0.0,0.5);
			gtk_grid_attach(GTK_GRID(dc_grid), button[row], 1, row, 1, 1);
			g_signal_connect(button[row], "clicked", G_CALLBACK(on_row),NULL);
			gtk_widget_show(button[row]);
			row++;
			
			
		}
	}
	
	if(cd_flag == 2)
	{
		char txt[] = ".txt";
		char actual_deck_aux[1024] = "q";
		char actual_deck_aux2[1024] = "q";
		strcpy(actual_deck_aux,actual_deck);
		strcat(actual_deck_aux,txt);
		
		delete_rows();
		
		FILE *f1 = fopen(actual_deck_aux, "r");
		strcat(actual_deck_aux2,actual_deck_aux);
		FILE *f2 = fopen(actual_deck_aux2, "r");
		if(f1==NULL||f2==NULL)
		{
			printf("File error!\n");
			exit(1);
		}
		
		//make buttons and append to grid
		row=0;
		while(1)
		{
			if(fgets(tmp,1024,f1)==NULL||fgets(tmp2,1024,f2)==NULL)
			{
				fclose(f1);
				fclose(f2);
				break;
			}
			tmp[strlen(tmp)-1] = 0;
			tmp2[strlen(tmp2)-1] = 0;
			
			gtk_grid_insert_row(GTK_GRID(dc_grid), row);
			strcat(tmp2,"x");
			strcat(tmp2,tmp);
			
			button[row] = gtk_button_new_with_label(tmp2);
			gtk_button_set_alignment(GTK_BUTTON(button[row]),0.0,0.5);
			gtk_grid_attach(GTK_GRID(dc_grid), button[row], 1, row, 1, 1);
			g_signal_connect(button[row], "clicked", G_CALLBACK(on_row2),NULL);
			gtk_widget_show(button[row]);
			row++;
			
			
		}
	}
	if(cd_flag == 3)
	{
		char txt[] = ".txt";
		char actual_deck_aux[1024] = "q";
		char actual_deck_aux2[1024] = "q";
		strcpy(actual_deck_aux,"AllCards");
		strcat(actual_deck_aux,txt);
		
		delete_rows();
		
		FILE *f1 = fopen(actual_deck_aux, "r");
		strcat(actual_deck_aux2,actual_deck_aux);
		FILE *f2 = fopen(actual_deck_aux2, "r");
		if(f1==NULL||f2==NULL)
		{
			printf("File error!\n");
			exit(1);
		}
		
		//make buttons and append to grid
		row=0;
		while(1)
		{
			if(fgets(tmp,1024,f1)==NULL||fgets(tmp2,1024,f2)==NULL)
			{
				fclose(f1);
				fclose(f2);
				break;
			}
			tmp[strlen(tmp)-1] = 0;
			tmp2[strlen(tmp2)-1] = 0;
			
			gtk_grid_insert_row(GTK_GRID(dc_grid), row);
			strcat(tmp2,"x");
			strcat(tmp2,tmp);
			
			button[row] = gtk_button_new_with_label(tmp2);
			gtk_button_set_alignment(GTK_BUTTON(button[row]),0.0,0.5);
			gtk_grid_attach(GTK_GRID(dc_grid), button[row], 1, row, 1, 1);
			g_signal_connect(button[row], "clicked", G_CALLBACK(on_row2),NULL);
			gtk_widget_show(button[row]);
			row++;
			
			
		}
	}
	
}

void on_row2(GtkButton *b)
{
	if(rem_flag==1)
	{
		char buff[100];
			int val_n = 0;
			while(1)
			{
				
				GtkWidget* temp = gtk_grid_get_child_at(GTK_GRID(dc_grid),1,val_n);
				if(GTK_BUTTON(temp) == b)
					break;
				
				val_n++;
				
				
			}
			sprintf(buff,"%d",val_n);
			
			
			
			system("rm aux.txt");
			char cmd_append[] = "cp ";
			strcat(cmd_append,"q");
			strcat(cmd_append,actual_deck);
			strcat(cmd_append,".txt ");
			strcat(cmd_append,"aux.txt");
			system(cmd_append);
			
			FILE *f1 = fopen("aux.txt", "r");
			
			if(f1==NULL)
			{
				printf("File error!\n");
				return;
			}
			
			
			char cmd_append2[] = "rm ";
			strcat(cmd_append2,"q");
			strcat(cmd_append2,actual_deck);
			strcat(cmd_append2,".txt");
			system(cmd_append2);
			
			char cmd_append3[] = "touch ";
			strcat(cmd_append3,"q");
			strcat(cmd_append3,actual_deck);
			strcat(cmd_append3,".txt");
			system(cmd_append3);
			
			row=0;
			while(1)
			{
				
				if(fgets(tmp,1024,f1)==NULL)
				{
					fclose(f1);
					break;
				}
				tmp[strlen(tmp)-1] = 0;
				char cmd_append4[2048] = "echo ";
				
				if(row == atoi(buff))
				{
					
					
					
					int val = atoi(tmp);
					val--;
					
					
					
					sprintf(tmp,"%d",val);
					strcat(cmd_append4,tmp);
					
					
					strcat(cmd_append4," >> ");
					strcat(cmd_append4,"q");
					strcat(cmd_append4,actual_deck);
					strcat(cmd_append4,".txt");
					
					
					system(cmd_append4);
					
					if(val == 0)
					{
						char buff2[100];
						sprintf(buff2,"%d",atoi(buff)+1);
						char cmd_append12[]="cp ";
						strcat(cmd_append12,actual_deck);
						strcat(cmd_append12,".txt");
						strcat(cmd_append12," aux2.txt");
						
						system(cmd_append12);
						
						char cmd_append11[]="sed '";
						strcat(cmd_append11,buff2);
						strcat(cmd_append11,"d' ");
						strcat(cmd_append11,"aux2.txt > ");
						strcat(cmd_append11,actual_deck);
						strcat(cmd_append11,".txt");
						printf("cmd:11%s\n",cmd_append11);
						system(cmd_append11);
						
						char cmd_append14[]="cp q";
						strcat(cmd_append14,actual_deck);
						strcat(cmd_append14,".txt");
						strcat(cmd_append14," aux2.txt");
						
						system(cmd_append14);
						
						char cmd_append13[]="sed '";
						strcat(cmd_append13,buff2);
						strcat(cmd_append13,"d' ");
						strcat(cmd_append13,"aux2.txt > q");
						strcat(cmd_append13,actual_deck);
						strcat(cmd_append13,".txt");
					
						system(cmd_append13);
					}
					
					
				}
				else
				{
				
					
					strcat(cmd_append4,tmp);
			
					
					system(cmd_append4);
					strcat(cmd_append4," >> ");
					strcat(cmd_append4,"q");
					strcat(cmd_append4,actual_deck);
					strcat(cmd_append4,".txt");
					
					system(cmd_append4);
				}
				row++;
			}
			update_list();
	
	}
	else
	{
			char buff[100];
			int val_n = 0;
			while(1)
			{
				
				GtkWidget* temp = gtk_grid_get_child_at(GTK_GRID(dc_grid),1,val_n);
				if(GTK_BUTTON(temp) == b)
					break;
				
				val_n++;
				
				
				
			}
			sprintf(buff,"%d",val_n);
		
			
			
			system("rm aux.txt");
			char cmd_append[] = "cp ";
			strcat(cmd_append,"q");
			strcat(cmd_append,actual_deck);
			strcat(cmd_append,".txt ");
			strcat(cmd_append,"aux.txt");
			system(cmd_append);
			
			FILE *f1 = fopen("aux.txt", "r");
			if(f1==NULL)
			{
				printf("File error!\n");
				return;
			}
			
			
			char cmd_append2[] = "rm ";
			strcat(cmd_append2,"q");
			strcat(cmd_append2,actual_deck);
			strcat(cmd_append2,".txt");
			system(cmd_append2);
			
			char cmd_append3[] = "touch ";
			strcat(cmd_append3,"q");
			strcat(cmd_append3,actual_deck);
			strcat(cmd_append3,".txt");
			system(cmd_append3);
			
			row=0;
			while(1)
			{
				
				if(fgets(tmp,1024,f1)==NULL)
				{
					fclose(f1);
					break;
				}
				tmp[strlen(tmp)-1] = 0;
				char cmd_append4[2048] = "echo ";
				
				if(row == atoi(buff))
				{
					
					
					
					int val = atoi(tmp);
					val++;
					
					sprintf(tmp,"%d",val);
					strcat(cmd_append4,tmp);
					
					
					strcat(cmd_append4," >> ");
					strcat(cmd_append4,"q");
					strcat(cmd_append4,actual_deck);
					strcat(cmd_append4,".txt");
					
					
					system(cmd_append4);
					
					
					
				}
				else
				{
				
					
					strcat(cmd_append4,tmp);
					
					
					system(cmd_append4);
					strcat(cmd_append4," >> ");
					strcat(cmd_append4,"q");
					strcat(cmd_append4,actual_deck);
					strcat(cmd_append4,".txt");
					
					system(cmd_append4);
				}
				row++;
			}
			update_list();
	}
	
}

void on_camera_btn_clicked (GtkButton *b)
{
	flag_card = cards();
	if(flag_card > 0){
			
			
			
			char txt[] = ".txt";
			char card_name[1024];
			
			char file_name[1024], file_namecp[1024];
			char append_cmd[1024] = "echo ";
			char append_cmd2[1024] = "echo ";
			char actual_deck_q[1024] = "q";
			char actual_deck_aux[1024] = "q";
			char actual_deck_aux2[1024] = "q";
			
			strcpy(actual_deck_aux,actual_deck);
			strcpy(actual_deck_aux2,actual_deck);
			
			
			strcat(actual_deck_aux, txt);
			
 
			
			//add card to deck
			strcat(append_cmd, nome2);
			strcat(append_cmd, ">>");
			strcat(append_cmd, actual_deck_aux);
			system(append_cmd);
			
			strcat(append_cmd2, "1");
			strcat(append_cmd2, " >> ");
			strcat(actual_deck_q,actual_deck_aux2);
			strcat(actual_deck_q,txt);
			
			strcat(append_cmd2, actual_deck_q);
			
			system(append_cmd2);
			update_list();
		}
}
void on_manual_btn_clicked (GtkButton *b)
{
	gtk_widget_show(text_entry);
	
}
void on_back_btn2_clicked (GtkButton *b)
{
	gtk_widget_hide(method_selector);
}


int cards() {
	
	
    quadro();
    tesse();
    
    FILE *f1 = fopen("names2.txt", "r");
		if(f1==NULL)
		{
			printf("names2.txt nao existe\n");
			exit(1);
		}
		
		//search for card
		printf("nome =>%s\n", nome2);
		while(1)
		{
			if(fgets(tmp,1024,f1)==NULL)
			{
				printf("Carta nao localizada!\n ");
				break;
			}
			tmp[strlen(tmp)-1] = 0;
			int diff = comparech(tmp,nome2);
			
			if (diff>80)
			{
				strcpy(nome2,tmp);
				printf("Carta localizada!---->%s\n ",nome2);
				fclose(f1);
				return 1;
			}
		}

fclose(f1);



return -1;
}
char tesse()
{

char *outText;
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  // Initialize tesseract-ocr with English, without specifying tessdata path
 if (api->Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
    }
  Pix *image = pixRead("sample.png");
  api->SetPageSegMode(tesseract::PSM_AUTO);
  api->SetImage(image);
  api->Recognize(0);
  Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
  //printf("Found %d textline image components.\n", boxes->n);
  char* ocrResult;
  for (int i = 0; i < boxes->n; i++) {
    BOX* box = boxaGetBox(boxes, i, L_CLONE);
    api->SetRectangle(box->x, box->y, box->w, box->h);
    ocrResult = api->GetUTF8Text();
    int conf = api->MeanTextConf();
    //fprintf(stdout, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s",
                    //i, box->x, box->y, box->w, box->h, conf, ocrResult);
    boxDestroy(&box);
  }
  int a;
  //if(ocrResult==NULL)
	//strcpy(nome2,"A");
  printf("OCR->%s\n", ocrResult);
  // Destroy used object and release memory
  api->End();

  
  delete api;
  
  
  
  //delete [] outText;
 
  pixDestroy(&image);
  char lixo[]="A";
  
  if(ocrResult==NULL)
		nome2=lixo;
  else
		nome2 = ocrResult;
  
  printf("nome2 = ocrResult\n");
  
  return 0;
}
int quadro()
{
	int i =0;

	Mat frame;
	Mat resized_frame;
	Mat grayscale;
	Mat grayscalecp;
	
	char *outText;


	//int x=980;
	//int y=1230;
	//int u=580;
	//int l=63;
	int x=970;
	int y=1230;
	int u=580;
	int l=63;
	//Rect text_rect(x,y,200,20);
	
	system("raspistill -o image.jpg -br 50 -sh 100 -sa -100 -ex auto -t 700 -q 100");

	frame = imread("image.jpg");
	
	Rect cut(x,y,u,l);
	imwrite("aaaaa.png",frame(cut));
	
	cvtColor(frame(cut),grayscale,COLOR_RGB2GRAY);
	threshold(grayscale ,grayscale ,200 ,255, THRESH_BINARY+THRESH_OTSU );
	//double scale_up_x = 3;
	//double scale_up_y = 3;
	//Mat scaled_f_up;
	//resize(grayscale, scaled_f_up, Size(), scale_up_x, scale_up_y, INTER_NEAREST);
	imwrite("sample.png",grayscale);

	
	return 0;
}


int tesste(char* name) {
 FILE *N_1 = NULL;
  N_1 = fopen("names.txt","r");
            if(N_1 == NULL){
                printf("Erro 2: Arquivo de Referência vocabulario_nota1  não pode ser aberto\n");
            }

char* buffer1 =0;
long length1;
if(N_1)
{
    fseek(N_1,0,SEEK_END);
    length1 = ftell(N_1);
    fseek(N_1,0,SEEK_SET);
    buffer1 = (char*)malloc(length1);
    if(buffer1)
    {
        fread(buffer1,1,length1,N_1);
    }
    fclose(N_1);
}
   if (strstr(buffer1,name)!=NULL){
            printf("Carta localizada!\n");
			return 0;
        }
   else
   printf("Por favor tente novamente");    
   free(buffer1);
return -1;

}

int comparech(const char* str1,const char* str2)
{
    int diff =0;
    int str1_size = (int)strlen(str1);//tmp
    int str2_size = (int)strlen(str2);//input
    if(str2_size+1<str1_size)
    {
		return 0;
	}
    if(strlen(str1)>strlen(str2))
    {
        
        for(int i=0;i<strlen(str2);i++)
        {
            if(str1[i]-str2[i]!=0)
            {
                diff++;
            }
        }
        
       
        return 100 - (diff*100)/str1_size;
    }
    else
    {
         for(int i=0;i<strlen(str1);i++)
        {
            if(str1[i]-str2[i]!=0)
            {
                diff++;
            }
        }
        
        
        return 100 - (diff*100)/str1_size;
    }
}

