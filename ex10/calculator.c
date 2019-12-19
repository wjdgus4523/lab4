#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

GtkWidget *buttAdd, *buttSub, *buttMul, *buttDiv;
GtkWidget *entry_A, *entry_B;
GtkWidget *message_label;

void quit ( GtkWidget *window, gpointer data)

{
gtk_main_quit();
}

void button_clicked(GtkWidget *button, gpointer data)

{
int a,b;
int result=0;
char resultMsg[80];
const char *textA=gtk_entry_get_text(GTK_ENTRY(entry_A));
const char *textB=gtk_entry_get_text(GTK_ENTRY(entry_B));

a=atoi(textA);
b=atoi(textB);

if(a==0 || b==0){
gtk_label_set_text(GTK_LABEL(message_label),"숫자 입력.");
return;

}

if(button==buttAdd)
result=a+b;
if(button==buttSub)
result=a-b;
if(button==buttMul)
result=a*b;

sprintf(resultMsg,"결과 =  %d",result);
if(button==buttDiv){
double resultf=(double)a/b;
sprintf(resultMsg,"결과 =  %.2lf",resultf);

}

 
gtk_label_set_text(GTK_LABEL(message_label),resultMsg);
}

int main (int argc, char *argv[])

{

GtkWidget *window;
GtkWidget *label_A, *label_B;
GtkWidget *hbox_1, *hbox_2,*hbox_3,*hbox_4;
GtkWidget *vbox;

gtk_init(&argc, &argv);
window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_title(GTK_WINDOW(window), "계산기" );
gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
gtk_window_set_default_size(GTK_WINDOW(window), 200, 150);
g_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (quit), NULL);
label_A = gtk_label_new("숫자1");
label_B = gtk_label_new("숫자2");
entry_A = gtk_entry_new();
entry_B = gtk_entry_new();
message_label = gtk_label_new("숫자 입력.");
buttAdd = gtk_button_new_with_label ("더하기");
buttSub = gtk_button_new_with_label ("빼기");
buttMul = gtk_button_new_with_label ("곱하기");
buttDiv = gtk_button_new_with_label ("나누기");
hbox_1 = gtk_hbox_new ( TRUE, 5 );
hbox_2 = gtk_hbox_new ( TRUE, 5 );
hbox_3 = gtk_hbox_new ( TRUE, 5 );
hbox_4 = gtk_hbox_new ( TRUE, 5 );
vbox = gtk_vbox_new ( FALSE, 10);
gtk_box_pack_start(GTK_BOX(hbox_1), label_A, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_1), entry_A, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_2), label_B, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_2), entry_B, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_3), message_label,TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_4), buttAdd, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_4), buttSub, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_4), buttMul, TRUE, FALSE, 5);
gtk_box_pack_start(GTK_BOX(hbox_4), buttDiv, TRUE, FALSE, 5);

 
gtk_box_pack_start(GTK_BOX(vbox),hbox_1,FALSE, FALSE, 10);
gtk_box_pack_start(GTK_BOX(vbox),hbox_2,FALSE, FALSE, 10);
gtk_box_pack_start(GTK_BOX(vbox),hbox_3,FALSE, FALSE, 10);
gtk_box_pack_start(GTK_BOX(vbox),hbox_4,FALSE, FALSE, 10);

 
gtk_container_add(GTK_CONTAINER(window), vbox);
g_signal_connect(GTK_OBJECT(buttAdd),"clicked", GTK_SIGNAL_FUNC(button_clicked),NULL);
g_signal_connect(GTK_OBJECT(buttSub),"clicked", GTK_SIGNAL_FUNC(button_clicked),NULL);
g_signal_connect(GTK_OBJECT(buttMul),"clicked", GTK_SIGNAL_FUNC(button_clicked),NULL);
g_signal_connect(GTK_OBJECT(buttDiv),"clicked", GTK_SIGNAL_FUNC(button_clicked),NULL);
gtk_widget_show_all(window);

gtk_main ();
return 0;
}
