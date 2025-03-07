#include <gtk/gtk.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ERROR_INVALID_INPUT 1

typedef struct
{
    const char *button_label;
    GtkLabel *display;
    double *nums;
    int *nums_index;
    int *str_index;
    bool *has_decimal_point;
} ButtonData;

typedef struct
{
    double *nums;
    int *nums_index;
    int *str_index;
    bool *has_decimal_point;
} DynamicData;

void slice_string(const char *str, int start, int end, char *result)
{
    if (start < 0 || end < start || end > strlen(str))
    {
        printf("Invalid indices\n");
        return;
    }

    strncpy(result, str + start, end - start); // Copy substring
    result[end - start] = '\0';                // Add null terminator
}

int save_new_number(const char *current_text, int *str_index, double *nums, int *nums_index)
{
    char new_num[100];

    // Getting new number
    slice_string(current_text, *str_index, strlen(current_text), new_num);

    g_print("Slice from %d to %lu: %s\n", *str_index, strlen(current_text), new_num);

    if (strlen(new_num) == 0)
    {
        return ERROR_INVALID_INPUT;
    }

    *str_index = strlen(current_text) + 1;
    g_print("New string index: %d\n", *str_index);

    g_print("New num: %s\n", new_num);

    char *end;
    double num = strtod(new_num, &end); // Converts base-10 string to long int
    if (*end != '\0')
    {
        g_print("Invalid number: extra characters found: %s\n", end);
    }
    else
    {
        g_print("Converted number: %lf\n", num);
    }

    nums[*nums_index] = num;
    g_print("nums_index: %d\n", *nums_index);
    *nums_index += 1;

    g_print("Nums: ");
    for (int i = 0; i < *nums_index; i++)
    {
        g_print("%lf ", nums[i]);
    }

    return 0;
}

void process_equation(const char *current_text, int *str_index, double *nums, int *nums_index, GtkLabel *display)
{
    double temp[100];
    int nums_pos = -1;
    int temp_pos = -1;
    bool ignore = FALSE;

    if (save_new_number(current_text, str_index, nums, nums_index) == ERROR_INVALID_INPUT)
    {
        return;
    }

    for (int i = 0; i < strlen(current_text); i++)
    {
        if (!isdigit(current_text[i]) && current_text[i] != '.')
        {
            nums_pos++;
            if (current_text[i] == '*')
            {
                if (ignore)
                {
                    temp[temp_pos] *= nums[nums_pos + 1];
                    continue;
                }
                temp_pos++;
                temp[temp_pos] = nums[nums_pos] * nums[nums_pos + 1];
                ignore = TRUE;
            }
            else if (current_text[i] == '/')
            {
                if (nums[nums_pos + 1] == 0)
                {
                    gtk_label_set_text(display, "Error");
                    return;
                }
                if (ignore)
                {
                    temp[temp_pos] /= nums[nums_pos + 1];
                    continue;
                }
                temp_pos++;
                temp[temp_pos] = nums[nums_pos] / nums[nums_pos + 1];
                ignore = TRUE;
            }
            else if (!ignore)
            {
                temp_pos++;
                temp[temp_pos] = nums[nums_pos];
            }
            else if (nums_pos + 2 == 100)
            {
                temp_pos++;
                temp[temp_pos] = nums[nums_pos + 1];
            }
            else
            {
                ignore = FALSE;
            }
        }
        else if (nums_pos + 2 == *nums_index && !ignore)
        {
            temp_pos++;
            temp[temp_pos] = nums[nums_pos + 1];
        }
    }

    for (int i = 0; i < temp_pos + 1; i++)
    {
        g_print("Temp[%d]: ", i);
        g_print("%lf\n", temp[i]);
    }

    int pos = 0;
    for (int i = 0; i < strlen(current_text); i++)
    {
        if (current_text[i] == '+')
        {
            pos++;
            temp[0] += temp[pos];
        }
        else if (current_text[i] == '-')
        {
            pos++;
            temp[0] -= temp[pos];
        }
    }

    char result[100];
    sprintf(result, "%.15g", temp[0]);
    gtk_label_set_text(display, result);

    nums[0] = temp[0];
    *nums_index = 0;
    *str_index = 0;
}

static void on_button_clicked(GtkWidget *widget, gpointer data)
{
    ButtonData *button_data = (ButtonData *)data;
    const char *button_label = button_data->button_label;
    GtkLabel *display = button_data->display;
    double *nums = button_data->nums;
    int *nums_index = button_data->nums_index;
    int *str_index = button_data->str_index;
    bool *has_decimal_point = button_data->has_decimal_point;

    g_print("Button %s clicked\n", button_label);

    const char *current_text = gtk_label_get_text(display);
    char display_text[100];
    bool is_cur_text_0 = FALSE;

    if (button_label == "C")
    {
        gtk_label_set_text(display, "0");
        *has_decimal_point = false;
        nums[0] = 0;
        *nums_index = 0;
        *str_index = 0;
        return;
    }
    else if (button_label == "CE")
    {
        int last_index = strlen(current_text);
        if (isdigit(current_text[last_index - 1]))
        {
            slice_string(current_text, 0, *str_index, display_text);
        }
        else
        {
            char *result;
            sprintf(result, "%.15g", nums[*nums_index - 1]);
            slice_string(current_text, 0, *str_index - 1, display_text);
            *str_index = strlen(display_text) - strlen(result);
            *nums_index -= 1;
            nums[*nums_index] = 0;
        }
        if (display_text[0] == '\0')
        {
            display_text[0] = '0';
            display_text[1] = '\0';
        }
        gtk_label_set_text(display, display_text);
        return;
    }
    else if (strcmp(current_text, "Error") == 0)
    {
        *has_decimal_point = false;
        return;
    }
    else if (button_label == "*" || button_label == "/" || button_label == "+" || button_label == "-")
    {
        *has_decimal_point = false;
        if (save_new_number(current_text, str_index, nums, nums_index) == ERROR_INVALID_INPUT)
        {
            return;
        }
    }
    else if (button_label == "=")
    {
        *has_decimal_point = false;
        process_equation(current_text, str_index, nums, nums_index, display);
        return;
    }
    else if (button_label == ".")
    {
        if (*has_decimal_point)
        {
            return;
        }
        *has_decimal_point = true;
    }
    else
    {
        // Handle number input

        // If the current text is "0", replace it with the button label
        // Otherwise, append the button label to the current text
        if (strlen(current_text) == 1 && current_text[0] == '0')
        {
            display_text[0] = '\0';
            is_cur_text_0 = TRUE;
        }
    }

    if (!is_cur_text_0)
    {
        // Copy the current text to the display text buffer
        strncpy(display_text, current_text, sizeof(display_text) - 1);
        display_text[sizeof(display_text) - 1] = '\0';
    }

    strncat(display_text, button_label, sizeof(display_text) - strlen(display_text) - 1);

    gtk_label_set_text(display, display_text);
    g_print("Display: %s\n", display_text);
}

static void create_button_data(GtkWidget *button, ButtonData *button_data, const char *button_label, GtkLabel *display, double *nums, int *nums_index, int *str_index, bool *has_decimal_point)
{
    button_data->button_label = button_label;
    button_data->display = display;
    button_data->nums = nums;
    button_data->nums_index = nums_index;
    button_data->str_index = str_index;
    button_data->has_decimal_point = has_decimal_point;

    g_object_set_data(G_OBJECT(button), "button_data", button_data);
}

static GtkWidget *create_button(char *label, GtkLabel *display, double *nums, int *nums_index, int *str_index, bool *has_decimal_point)
{
    GtkWidget *button = gtk_button_new_with_label(label);

    ButtonData *button_data = g_new(ButtonData, 1);

    create_button_data(button, button_data, label, display, nums, nums_index, str_index, has_decimal_point);

    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), button_data);
    return button;
}

static void on_window_destroy(GtkWidget *widget, gpointer data)
{
    DynamicData *data_pointers = (DynamicData *)data;
    double *nums = data_pointers->nums;
    int *nums_index = data_pointers->nums_index;
    int *str_index = data_pointers->str_index;
    bool *has_decimal_point = data_pointers->has_decimal_point;

    g_free(nums);
    g_free(nums_index);
    g_free(str_index);
    g_free(has_decimal_point);

    g_print("Memory freed.\n");
}

static void
activate(GtkApplication *app,
         gpointer user_data)
{
    GtkWidget *window, *grid, *display;
    GtkCssProvider *css_provider;
    GdkDisplay *css_display;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Calculadora");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 300);

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    display = gtk_label_new("0");
    gtk_label_set_xalign(GTK_LABEL(display), 1);
    gtk_grid_attach(GTK_GRID(grid), display, 0, 0, 4, 1);
    gtk_widget_add_css_class(display, "display");

    char *buttons[] = {"7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", ".", "0", "=", "+"};
    double *nums = g_new(double, 100);
    int *nums_index = g_new(int, 1);
    int *str_index = g_new(int, 1);
    bool *has_decimal_point = g_new(bool, 1);
    *nums_index = 0;
    *str_index = 0;
    *has_decimal_point = false;

    GtkWidget *clear_button = create_button("C", GTK_LABEL(display), nums, nums_index, str_index, has_decimal_point);
    gtk_grid_attach(GTK_GRID(grid), clear_button, 0, 1, 1, 1);

    GtkWidget *clear_one_button = create_button("CE", GTK_LABEL(display), nums, nums_index, str_index, has_decimal_point);
    gtk_grid_attach(GTK_GRID(grid), clear_one_button, 3, 1, 1, 1);

    for (int i = 0; i < 16; i++)
    {
        GtkWidget *button = create_button(buttons[i], GTK_LABEL(display), nums, nums_index, str_index, has_decimal_point);

        gtk_grid_attach(GTK_GRID(grid), button, i % 4, (i + 8) / 4, 1, 1);
    }

    css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_file(css_provider, g_file_new_for_path("style.css"));
    css_display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(css_display, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    DynamicData data_pointers = {nums, nums_index, str_index, has_decimal_point};
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), &data_pointers);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc,
         char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.calc", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}