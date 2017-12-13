#include <gtk/gtk.h>
//gcc `pkg-config --cflags gtk+-3.0` -o layout layout.c `pkg-config --libs gtk+-3.0`
GdkPixbuf *create_pixbuf(const gchar *filename)
{
  // set icon for this window
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  pixbuf = gdk_pixbuf_new_from_file(filename, &error);

  if (!pixbuf)
  {

    fprintf(stderr, "%s\n", error->message);
    g_error_free(error);
}

return pixbuf;
}

int main(int argc, char *argv[])
{
    GtkBuilder *gtkBuilder;
    GtkWidget *window;
    gtk_init(&argc, &argv);
    
    gtkBuilder = gtk_builder_new();
    gtk_builder_add_from_file(gtkBuilder, "layout.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(gtkBuilder, "window"));
    
    g_object_unref(G_OBJECT(gtkBuilder));
    gtk_widget_show(window);
    gtk_window_set_title(GTK_WINDOW(window), "Black & white");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    gtk_main();
    
    return 0;
}
