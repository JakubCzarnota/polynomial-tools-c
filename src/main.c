#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <curses.h>
#else // linux/macOS
#include <ncurses.h>
#include <unistd.h>
#endif

#include "polynomial.h"
#include "string_utils.h"
#include "plot.h"
#include "bmp.h"
#include "sturm_sequence.h"

#define MAX_EXPR 1024
#define DELEY_SLEEP 500
#define MAX_POLYNOMIAL_DEGREE 20
#define MAX_POLYNOMIAL_COEFFICIENT 1000
#define MIN_POLYNOMIAL_COEFFICIENT -1000

static const char *POLY_LOGO[] = {
    "  _____   ____  _  __     ___   _  ____  __  __ _____          _                  _____  _____  ",
    " |  __ \\ / __ \\| | \\ \\   / / \\ | |/ __ \\|  \\/  |_   _|   /\\   | |           /\\   |  __ \\|  __ \\ ",
    " | |__) | |  | | |  \\ \\_/ /|  \\| | |  | | \\  / | | |    /  \\  | |          /  \\  | |__) | |__) |",
    " |  ___/| |  | | |   \\   / | . ` | |  | | |\\/| | | |   / /\\ \\ | |         / /\\ \\ |  ___/|  ___/ ",
    " | |    | |__| | |____| |  | |\\  | |__| | |  | |_| |_ / ____ \\| |____    / ____ \\| |    | |     ",
    " |_|     \\____/|______|_|  |_| \\_|\\____/|_|  |_|_____/_/    \\_\\______|  /_/    \\_\\_|    |_|     "};
#define POLY_LOGO_LINES 6

void main_menu(void);
void help_menu(void);
void polynomial_menu(void);
void draw_polynomial_plot_menu(void);

Polynomial p1;
Polynomial p2;

int selected_polynomial = 1;

bool polynomial1_defined = false;
bool polynomial2_defined = false;

char poly1_formula[MAX_EXPR] = "";
char poly2_formula[MAX_EXPR] = "";

char message[1024] = "";

void sleep_ms(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void set_escape_delay(int ms)
{
#ifdef _WIN32
    // ESCDELAY = ms;
#else
    set_escdelay(ms);
#endif
}

bool is_polynomial_valid(Polynomial *p)
{
    if (p->degree > MAX_POLYNOMIAL_DEGREE)
        return false;

    for (int i = 0; i < p->degree + 1; i++)
    {
        if (p->coefficients[i] > MAX_POLYNOMIAL_COEFFICIENT || p->coefficients[i] < MIN_POLYNOMIAL_COEFFICIENT)
            return false;
    }

    return true;
}

int main()
{
    initscr();
    set_escape_delay(25);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    main_menu();

    endwin();

    if (polynomial1_defined)
        free_polynomial(&p1);

    if (polynomial2_defined)
        free_polynomial(&p2);

    return 0;
}

void main_menu()
{
    int choice = 0;
    char *options[] = {
        "Polynomial",
        "Help",
        "Exit"};

    while (1)
    {
        clear();
        int h, w;
        getmaxyx(stdscr, h, w);

        // Draw logo
        int logo_start_y = h / 2 - POLY_LOGO_LINES - 4;
        if (logo_start_y < 1)
            logo_start_y = 1;

        attron(A_BOLD);
        for (int i = 0; i < POLY_LOGO_LINES; i++)
        {
            int x = (w - strlen(POLY_LOGO[i])) / 2;
            mvprintw(logo_start_y + i, x, "%s", POLY_LOGO[i]);
        }
        attroff(A_BOLD);

        int menu_start_y = logo_start_y + POLY_LOGO_LINES + 6;

        for (int i = 0; i < 3; i++)
        {
            int x = (w - strlen(options[i])) / 2;
            int y = menu_start_y + i * 2;

            if (i == choice)
                attron(A_REVERSE);

            mvprintw(y, x, "%s", options[i]);

            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        switch (ch)
        {
        case KEY_UP:
            if (choice > 0)
                choice--;
            break;
        case KEY_DOWN:
            if (choice < 2)
                choice++;
            break;
        case '\n':
            if (choice == 0)
                polynomial_menu();
            else if (choice == 1)
                help_menu();
            else
                return;
            break;
        case 27: // ESC
            return;
        default:
            break;
        }
    }
}
void help_menu()
{
    int h, w;
    getmaxyx(stdscr, h, w);

    WINDOW *help_win = newwin(h - 4, w - 4, 2, 2);
    box(help_win, 0, 0);
    keypad(help_win, TRUE);

    wattron(help_win, A_BOLD);
    mvwprintw(help_win, 1, 2, "HELP");
    wattroff(help_win, A_BOLD);

    mvwprintw(help_win, 3, 2, "Navigation:");
    mvwprintw(help_win, 4, 4, "UP / DOWN        - Move through menu options");
    mvwprintw(help_win, 5, 4, "ENTER            - Confirm selection");
    mvwprintw(help_win, 6, 4, "ESC              - Go back / Exit");

    mvwprintw(help_win, 8, 2, "Polynomial Menu:");
    mvwprintw(help_win, 9, 4, "LEFT  or Q       - Select Polynomial 1");
    mvwprintw(help_win, 10, 4, "RIGHT or W       - Select Polynomial 2");
    mvwprintw(help_win, 11, 4, "Type expression  - Enter polynomial formula (e.g. 2x^2-3x+1)");
    mvwprintw(help_win, 12, 4, "BACKSPACE        - Delete last character");
    mvwprintw(help_win, 13, 4, "ENTER            - Parse and analyze polynomial");
    mvwprintw(help_win, 14, 4, "R                - Reset Polynomial Menu");

    mvwprintw(help_win, 16, 2, "Operations (require both polynomials):");
    mvwprintw(help_win, 17, 4, "A                - Add polynomials");
    mvwprintw(help_win, 18, 4, "S                - Subtract polynomials");
    mvwprintw(help_win, 19, 4, "F                - Multiply polynomials");
    mvwprintw(help_win, 20, 4, "D                - Divide polynomials (shows remainder)");

    mvwprintw(help_win, 22, 2, "Plotting (require selected polynomial):");
    mvwprintw(help_win, 23, 4, "P                - Generate bitmap plot of selected polynomial");

    wattron(help_win, A_BOLD);
    mvwprintw(help_win, h - 6, 2, "Press any key to return to the main menu");
    wattroff(help_win, A_BOLD);

    wrefresh(help_win);
    wgetch(help_win);
    delwin(help_win);
}

double compute_polynomial_value_wrapper(double x)
{
    if (selected_polynomial == 1 && polynomial1_defined)
        return polynomial_evaluate(&p1, x);
    else if (selected_polynomial == 2 && polynomial2_defined)
        return polynomial_evaluate(&p2, x);
    else
        return 0.0;
}

void display_polynomial(WINDOW *win, int start_y, int start_x, const Polynomial *p)
{
    mvwprintw(win, start_y, start_x, "Polynomial formula: ");

    char poly_str[512] = "";
    polynomial_to_string(p, poly_str, sizeof(poly_str));
    mvwprintw(win, start_y + 1, start_x + 2, "%s", poly_str);

    mvwprintw(win, start_y + 3, start_x, "Degree: %d", p->degree);

    mvwprintw(win, start_y + 5, start_x, "Roots count: %d", p->roots.size);
    char buf[128] = "";
    root_array_list_to_string(&p->roots, buf, sizeof(buf));
    mvwprintw(win, start_y + 6, start_x, "Roots: %s", buf);

    buf[0] = '\0';
    interval_array_list_to_string(&p->positive_value_intervals, buf, sizeof(buf));
    mvwprintw(win, start_y + 8, start_x, "Positive Value Intervals: %s", buf);
    buf[0] = '\0';
    interval_array_list_to_string(&p->negative_value_intervals, buf, sizeof(buf));
    mvwprintw(win, start_y + 9, start_x, "Negative Value Intervals: %s", buf);

    buf[0] = '\0';
    point_array_list_to_string(&p->extreme_points, buf, sizeof(buf));
    mvwprintw(win, start_y + 11, start_x, "Extreme Points: %s", buf);
    buf[0] = '\0';
    interval_array_list_to_string(&p->monotonic_increasing_intervals, buf, sizeof(buf));
    mvwprintw(win, start_y + 13, start_x, "Increasing Intervals: %s", buf);

    buf[0] = '\0';
    interval_array_list_to_string(&p->monotonic_decreasing_intervals, buf, sizeof(buf));
    mvwprintw(win, start_y + 14, start_x, "Decreasing Intervals: %s", buf);
    buf[0] = '\0';
    point_array_list_to_string(&p->inflection_points, buf, sizeof(buf));
    mvwprintw(win, start_y + 16, start_x, "Inflection Points: %s", buf);

    buf[0] = '\0';
    interval_array_list_to_string(&p->concave_intervals, buf, sizeof(buf));
    mvwprintw(win, start_y + 18, start_x, "Concave Intervals: %s", buf);
    buf[0] = '\0';
    interval_array_list_to_string(&p->convex_intervals, buf, sizeof(buf));
    mvwprintw(win, start_y + 19, start_x, "Convex Intervals: %s", buf);

    buf[0] = '\0';
    ExtendedValue approach = (ExtendedValue){VALUE_NEG_INF, 0};
    ExtendedValue limit = polynomial_limit(p, approach);
    extended_value_to_string(&limit, buf, sizeof(buf));
    mvwprintw(win, start_y + 21, start_x, "Limit at -Inf: %s", buf);

    buf[0] = '\0';
    approach = (ExtendedValue){VALUE_POS_INF, 0};
    limit = polynomial_limit(p, approach);
    extended_value_to_string(&limit, buf, sizeof(buf));
    mvwprintw(win, start_y + 22, start_x, "Limit at +Inf: %s", buf);

    buf[0] = '\0';
    interval_to_string(&p->range, buf, sizeof(buf));
    mvwprintw(win, start_y + 24, start_x, "Range: %s", buf);
}

void polynomial_menu()
{

    int h, w;
    getmaxyx(stdscr, h, w);

    WINDOW *top = newwin(3 * h / 4, w, 0, 0);
    WINDOW *bottom = newwin(h / 4, w, 3 * h / 4, 0);

    keypad(bottom, TRUE);

    while (1)
    {
        // Top window - display polynomials info
        werase(top);
        box(top, 0, 0);

        int half_w = w / 2;

        // Left side - first polynomial
        if (selected_polynomial == 1)
            wattron(top, A_REVERSE);
        mvwprintw(top, 1, 2, "Polynomial 1");
        if (selected_polynomial == 1)
            wattroff(top, A_REVERSE);

        if (polynomial1_defined)
        {
            display_polynomial(top, 3, 2, &p1);
        }
        else
        {
            mvwprintw(top, 3, 2, "Not defined yet.");
        }

        // Vertical divider
        for (int i = 0; i < 3 * h / 4; i++)
            mvwaddch(top, i, half_w, ACS_VLINE);

        // Right side - second polynomial
        if (selected_polynomial == 2)
            wattron(top, A_REVERSE);
        mvwprintw(top, 1, half_w + 2, "Polynomial 2");
        if (selected_polynomial == 2)
            wattroff(top, A_REVERSE);

        if (polynomial2_defined)
        {
            display_polynomial(top, 3, half_w + 2, &p2);
        }
        else
        {
            mvwprintw(top, 3, half_w + 2, "Not defined yet.");
        }

        // Bottom window - input and commands
        werase(bottom);
        box(bottom, 0, 0);
        mvwprintw(bottom, 1, 2, "Enter polynomial formula for Polynomial %d (ENTER to submit):", selected_polynomial);

        if (selected_polynomial == 1)
            mvwprintw(bottom, 2, 4, "> %s_", poly1_formula);
        else if (selected_polynomial == 2)
            mvwprintw(bottom, 2, 4, "> %s_", poly2_formula);

        if (message[0] != '\0')
            mvwprintw(bottom, h / 4 - 6, 2, "%s", message);

        mvwprintw(bottom, h / 4 - 4, 2, "<- or Q - Select Polynomial 1, -> or W - Select Polynomial 2, R - Reset, A - Add, S - Subtract, D - Divide, F - Multiply");

        mvwprintw(bottom, h / 4 - 2, 2, "ESC to exit");

        wrefresh(top);
        wrefresh(bottom);

        int ch = wgetch(bottom);

        switch (ch)
        {
        case 27: // ESC
            goto end_loop;
        case KEY_LEFT:
        case 'q':
        case 'Q':
            selected_polynomial = 1;
            break;
        case KEY_RIGHT:
        case 'w':
        case 'W':
            selected_polynomial = 2;
            break;
        case 'r':
        case 'R':
            if (polynomial1_defined)
            {
                free_polynomial(&p1);
                polynomial1_defined = false;
                poly1_formula[0] = '\0';
            }

            if (polynomial2_defined)
            {
                free_polynomial(&p2);
                polynomial2_defined = false;
                poly2_formula[0] = '\0';
            }

            message[0] = '\0';
            break;
        case 'a':
        case 'A':
            if (!polynomial1_defined || !polynomial2_defined)
                break;

            if (selected_polynomial == 1)
            {
                Polynomial new_poly = polynomial_add(&p1, &p2);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p1);
                p1 = new_poly;

                polynomial_find_properties(&p1);

                poly1_formula[0] = '\0';
                polynomial_to_string(&p1, poly1_formula, sizeof(poly1_formula));
                remove_whitespace(poly1_formula);
            }
            else if (selected_polynomial == 2)
            {
                Polynomial new_poly = polynomial_add(&p2, &p1);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p2);
                p2 = new_poly;

                polynomial_find_properties(&p2);

                poly2_formula[0] = '\0';
                polynomial_to_string(&p2, poly2_formula, sizeof(poly2_formula));
                remove_whitespace(poly2_formula);
            }
            break;
        case 's':
        case 'S':
            if (!polynomial1_defined || !polynomial2_defined)
                break;

            if (selected_polynomial == 1)
            {
                Polynomial new_poly = polynomial_subtract(&p1, &p2);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p1);
                p1 = new_poly;

                polynomial_find_properties(&p1);

                poly1_formula[0] = '\0';
                polynomial_to_string(&p1, poly1_formula, sizeof(poly1_formula));
                remove_whitespace(poly1_formula);
            }
            else if (selected_polynomial == 2)
            {
                Polynomial new_poly = polynomial_subtract(&p2, &p1);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p2);
                p2 = new_poly;

                polynomial_find_properties(&p2);

                poly2_formula[0] = '\0';
                polynomial_to_string(&p2, poly2_formula, sizeof(poly2_formula));
                remove_whitespace(poly2_formula);
            }
            break;
        case 'd':
        case 'D':
            if (!polynomial1_defined || !polynomial2_defined)
                break;

            Polynomial rest;
            message[0] = '\0';

            if (selected_polynomial == 1)
            {

                Polynomial new_poly = polynomial_divide(&p1, &p2, &rest);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p1);
                p1 = new_poly;

                polynomial_find_properties(&p1);

                poly1_formula[0] = '\0';
                polynomial_to_string(&p1, poly1_formula, sizeof(poly1_formula));
                remove_whitespace(poly1_formula);
            }
            else if (selected_polynomial == 2)
            {
                Polynomial new_poly = polynomial_divide(&p2, &p1, &rest);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p2);
                p2 = new_poly;

                polynomial_find_properties(&p2);

                poly2_formula[0] = '\0';
                polynomial_to_string(&p2, poly2_formula, sizeof(poly2_formula));
                remove_whitespace(poly2_formula);
            }

            if (polynomial_is_zero(&rest) == false)
            {
                char rest_str[520] = "";
                polynomial_to_string(&rest, rest_str, sizeof(rest_str));
                snprintf(message, sizeof(message), "Rest: %s", rest_str);
            }

            free_polynomial(&rest);
            break;
        case 'f':
        case 'F':
            if (!polynomial1_defined || !polynomial2_defined)
                break;

            if (selected_polynomial == 1)
            {
                Polynomial new_poly = polynomial_multiply(&p1, &p2);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p1);
                p1 = new_poly;

                polynomial_find_properties(&p1);

                poly1_formula[0] = '\0';
                polynomial_to_string(&p1, poly1_formula, sizeof(poly1_formula));
                remove_whitespace(poly1_formula);
            }
            else if (selected_polynomial == 2)
            {
                Polynomial new_poly = polynomial_multiply(&p2, &p1);

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                free_polynomial(&p2);
                p2 = new_poly;

                polynomial_find_properties(&p2);

                poly2_formula[0] = '\0';
                polynomial_to_string(&p2, poly2_formula, sizeof(poly2_formula));
                remove_whitespace(poly2_formula);
            }
            break;
        case 'p':
        case 'P':
            if ((selected_polynomial == 1 && polynomial1_defined) || (selected_polynomial == 2 && polynomial2_defined))
                draw_polynomial_plot_menu();
            break;
        case KEY_BACKSPACE:
        case '\b':
        case 127:
        {
            char *formula;

            if (selected_polynomial == 1)
                formula = poly1_formula;
            else
                formula = poly2_formula;

            int len = strlen(formula);
            if (len > 0)
                formula[len - 1] = '\0';
            break;
        }
        case '\n':
            message[0] = '\0';

            if (selected_polynomial == 1)
            {

                Polynomial new_poly = create_polynomial_from_formula(poly1_formula, message, sizeof(message));

                if (message[0] != '\0')
                {
                    free_polynomial(&new_poly);
                    break;
                }

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                if (polynomial1_defined)
                    free_polynomial(&p1);

                p1 = new_poly;

                polynomial_find_properties(&p1);
                polynomial1_defined = true;
            }
            else if (selected_polynomial == 2)
            {
                Polynomial new_poly = create_polynomial_from_formula(poly2_formula, message, sizeof(message));

                if (message[0] != '\0')
                {
                    free_polynomial(&new_poly);
                    break;
                }

                if (!is_polynomial_valid(&new_poly))
                {
                    free_polynomial(&new_poly);
                    snprintf(message, sizeof(message), "Polynomial too big");
                    break;
                }

                if (polynomial2_defined)
                    free_polynomial(&p2);

                p2 = new_poly;

                polynomial_find_properties(&p2);
                polynomial2_defined = true;
            }
            break;
        default:
            if (strlen(poly1_formula) < MAX_EXPR - 1 && (isdigit((unsigned char)ch) || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == 'x' || ch == 'X' || ch == ' ' || ch == '.'))
            {
                char *formula = (selected_polynomial == 1) ? poly1_formula : poly2_formula;

                int len = strlen(formula);
                formula[len] = ch;
                formula[len + 1] = '\0';
            }
            break;
        }
    }

end_loop:

    delwin(top);
    delwin(bottom);
}

void get_path(WINDOW *win, char *buffer, int buffer_size, int y, int x)
{
    if (buffer == NULL || buffer_size <= 0)
        return;

    if (buffer_size < 6)
    {
        buffer[0] = '\0';
        return;
    }

    int ch;
    char *buf = malloc((buffer_size - 4) * sizeof(char));

    buf[0] = '\0';

    mvwprintw(win, y, x, "File name: .bpm");
    wclrtoeol(win);

    do
    {
        ch = wgetch(win);

        if ((ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) || ch == '_' || ch == '-')
        {
            int len = strlen(buf);

            if (len >= buffer_size - 6)
                continue;

            buf[len] = ch;
            buf[len + 1] = '\0';
            mvwprintw(win, y, x, "File name: %s.bmp", buf);
            wclrtoeol(win);
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            int len = strlen(buf);
            if (len > 0)
                buf[len - 1] = '\0';
            mvwprintw(win, y, x, "File name: %s.bmp", buf);
            wclrtoeol(win);
        }
        else if (ch == 27) // ESC
        {
            free(buf);
            buffer[0] = '\0';
            return;
        }
        else if (ch == '\n' && buf[0] != '\0')
            break;

    } while (1);

    snprintf(buffer, buffer_size, "%s.bmp", buf);
    free(buf);
}

int get_number(WINDOW *win, char *message, int y, int x)
{
    int ch;
    char buf[16] = "";

    mvwprintw(win, y, x, "%s: ", message);
    wclrtoeol(win);

    do
    {
        ch = wgetch(win);

        if (isdigit((unsigned char)ch))
        {
            size_t len = strlen(buf);
            if (len >= sizeof(buf) - 2)
                continue;

            buf[len] = ch;
            buf[len + 1] = '\0';
            mvwprintw(win, y, x, "%s: %s", message, buf);
            wclrtoeol(win);
        }
        else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            int len = strlen(buf);
            if (len > 0)
                buf[len - 1] = '\0';
            mvwprintw(win, y, x, "%s: %s", message, buf);
            wclrtoeol(win);
        }
        else if (ch == 27) // ESC
        {
            return -1;
        }
    } while (ch != '\n' || buf[0] == '\0');

    return atoi(buf);
}

void draw_polynomial_plot_menu()
{
    clear();
    int h, w;

    getmaxyx(stdscr, h, w);
    WINDOW *plot_win = newwin(h - 4, w - 4, 2, 2);
    keypad(plot_win, TRUE);

    box(plot_win, 0, 0);
    mvwprintw(plot_win, 1, 2, "POLYNOMIAL PLOT");

    char poly_str[512] = "";

    if (selected_polynomial == 1)
    {
        polynomial_to_string(&p1, poly_str, sizeof(poly_str));
    }
    else if (selected_polynomial == 2)
    {
        polynomial_to_string(&p2, poly_str, sizeof(poly_str));
    }

    mvwprintw(plot_win, 3, 2, "Plot window");

    mvwprintw(plot_win, 4, 2, "Polynomial: %s", poly_str);

    char path[256] = "";

    get_path(plot_win, path, sizeof(path), 5, 2);
    if (path[0] == '\0')
        goto exit_plotting;

    int width = get_number(plot_win, "Enter image width", 6, 2);
    if (width < 0)
        goto exit_plotting;

    int height = get_number(plot_win, "Enter image height", 7, 2);
    if (height < 0)
        goto exit_plotting;

    int x = get_number(plot_win, "Enter x (the function will be plotted from -x to x)", 8, 2);
    if (x < 0)
        goto exit_plotting;

    int y = get_number(plot_win, "Enter y (the function will be plotted from -y to y)", 9, 2);
    if (y < 0)
        goto exit_plotting;

    int thickness = get_number(plot_win, "Enter line thickness", 10, 2);
    if (thickness < 0)
        goto exit_plotting;

    unsigned char *image =
        malloc(width * height * 3);

    if (image == NULL)
    {
        mvwprintw(plot_win, 12, 2, "Failed to allocate memory for image.");
        mvprintw(h - 2, 2, "Press any key to continue...");
        wrefresh(plot_win);
        getch();
        delwin(plot_win);
        return;
    }

    mvwprintw(plot_win, 12, 2, "Plotting...");
    wrefresh(plot_win);

    plot_function(
        image,
        width,
        height,
        compute_polynomial_value_wrapper,
        -x, x,
        -y, y,
        thickness);

    sleep_ms(DELEY_SLEEP);

    mvwprintw(plot_win, 14, 2, "Saving to %s...", path);
    wrefresh(plot_win);

    bmp_write(path, width, height, image);
    free(image);

    sleep_ms(DELEY_SLEEP);

    mvwprintw(plot_win, 16, 2, "Image saved successfully. Press any key to continue...");
    wrefresh(plot_win);

    wgetch(plot_win);

exit_plotting:

    wrefresh(plot_win);
    delwin(plot_win);
}
