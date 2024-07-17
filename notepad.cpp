#include <gtk/gtk.h>

GtkWidget *text_view;

// Function to open a file and load its content into the text view
void open_file(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                          GTK_WINDOW(data),
                                          action,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);

        FILE *file = fopen(filename, "r");
        if (file != NULL) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
            GtkTextIter start, end;
            gtk_text_buffer_get_start_iter(buffer, &start);
            gtk_text_buffer_get_end_iter(buffer, &end);
            gchar *contents = NULL;
            gsize length = 0;
            g_file_get_contents(filename, &contents, &length, NULL);
            gtk_text_buffer_set_text(buffer, contents, -1);
            g_free(contents);
            fclose(file);
        }
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

// Function to save the text view content to a file
void save_file(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Save File",
                                          GTK_WINDOW(data),
                                          action,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Save",
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    chooser = GTK_FILE_CHOOSER (dialog);

    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename (chooser);

        FILE *file = fopen(filename, "w");
        if (file != NULL) {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
            GtkTextIter start, end;
            gtk_text_buffer_get_start_iter(buffer, &start);
            gtk_text_buffer_get_end_iter(buffer, &end);
            gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
            fwrite(text, 1, strlen(text), file);
            g_free(text);
            fclose(file);
        }
        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

// Function to create the GUI and set up the application
void activate(GtkApplication* app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *menu_bar;
    GtkWidget *file_menu;
    GtkWidget *edit_menu;
    GtkWidget *open_item;
    GtkWidget *save_item;
    GtkWidget *save_as_item;
    GtkWidget *copy_item;
    GtkWidget *paste_item;
    GtkWidget *scroll;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Hari's Notepad");
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Menu Bar
    menu_bar = gtk_menu_bar_new();

    // File Menu
    file_menu = gtk_menu_new();
    
    open_item = gtk_menu_item_new_with_label("Open");
    save_item = gtk_menu_item_new_with_label("Save");
    save_as_item = gtk_menu_item_new_with_label("Save As");

    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_item);

    GtkWidget *file_menu_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);

    // Edit Menu
    edit_menu = gtk_menu_new();
    copy_item = gtk_menu_item_new_with_label("Copy");
    paste_item = gtk_menu_item_new_with_label("Paste");

    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), copy_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), paste_item);

    GtkWidget *edit_menu_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);

    // Attach Menu Bar to Grid
    gtk_grid_attach(GTK_GRID(grid), menu_bar, 0, 0, 1, 1);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);  

    // Text View in Center
    text_view = gtk_text_view_new();
    gtk_widget_set_size_request (GTK_WIDGET (text_view), 600, 400);  // Set desired size

    // Allow text_view to expand within the grid
    g_object_set(G_OBJECT(text_view), "hexpand", TRUE, NULL);
    g_object_set(G_OBJECT(text_view), "vexpand", TRUE, NULL);

    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), text_view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 1, 1, 1);
    

    // Connect Signals
    g_signal_connect(G_OBJECT(open_item), "activate", G_CALLBACK(open_file), window);
    g_signal_connect(G_OBJECT(save_item), "activate", G_CALLBACK(save_file), window);
    g_signal_connect(G_OBJECT(save_as_item), "activate", G_CALLBACK(save_file), window);
    // Add more signal connections for other menu items if needed

    gtk_widget_show_all (window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new ("com.example.gtk_notepad", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}
