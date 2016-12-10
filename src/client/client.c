#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <ncurses.h>
#include <ctype.h>
#include <getopt.h>
#include "tchatche.h"
#include "tui.h"

static tui_msg messages[] = {
    {1000000000, "jby", "Lorem ipsum dolor sit amet, consectetur adipiscing elit."},
    {1000000120, "Jean-Raphael", "Nulla semper euismod volutpat."},
    {1000000240, "jby", "Integer quis tortor velit."},
    {1000000360, "jby", "Cras justo purus, tincidunt tincidunt iaculis porta, posuere et nibh."},
    {1000000480, "jby", "Pellentesque vel blandit dolor."},
    {1000000600, "Antonin", "Fusce neque tellus, ultrices ut lacinia ac, aliquet a est."},
    {1000000720, "Jean-Raphael", "Vivamus nisi mi, scelerisque sed metus id, venenatis ultricies nisl."},
    {1000000840, "jby", "Vivamus dignissim nec mi sed rutrum."},
    {1000000960, "Jean-Raphael", "Aliquam erat volutpat."},
    {1000001080, "jch", "Nulla facilisi."},
    {1000001200, "Jean-Raphael", "Aenean ut tellus mattis enim aliquet laoreet."},
    {1000001320, "jch", "In malesuada posuere velit vel viverra."},
    {1000001440, "Jean-Raphael", "Vestibulum mattis vestibulum tincidunt."},
    {1000001560, "jch", "Fusce vulputate leo nec elit congue, a pellentesque sapien rhoncus."},
    {1000001680, "jby", "Lorem ipsum dolor sit amet, consectetur adipiscing elit."},
    {1000001800, "jby", "Vestibulum congue, sem sit amet hendrerit scelerisque, libero neque volutpat leo, non suscipit neque arcu vel nulla."},
    {1000001920, "jch", "Suspendisse pellentesque odio dui, lacinia pharetra magna dignissim vitae."},
    {1000002040, "jch", "Praesent justo nulla, consectetur id arcu sit amet, tempor fringilla elit."},
    {1000002160, "Antonin", "Integer odio mi, varius vel ornare et, lobortis eget purus."},
    {1000002280, "jch", "Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus."},
    {1000002400, "Antonin", "Nunc nec vulputate neque."},
    {1000002520, "Antonin", "Duis imperdiet scelerisque consectetur."},
    {1000002640, "Antonin", "Cras molestie nisi quis sollicitudin finibus."},
    {1000002760, "jby", "Nullam vehicula porttitor auctor."},
    {1000002880, "Antonin", "Proin consectetur pharetra feugiat."},
    {1000003000, "Jean-Raphael", "Aliquam nisi sem, ullamcorper et lacinia at, sollicitudin vel dui."},
    {1000003120, "Antonin", "Cras eget euismod sem, in rutrum est."},
    {1000003240, "Antonin", "Interdum et malesuada fames ac ante ipsum primis in faucibus."},
    {1000003360, "jby", "Aenean ultrices turpis purus, et aliquet ex posuere sed."},
    {1000003480, "Antonin", "Aenean risus lacus, gravida quis tellus iaculis, molestie facilisis elit."},
    {1000003600, "jch", "Cras aliquet dolor at velit tincidunt porta."},
    {1000003720, "jby", "Phasellus sed iaculis dolor."},
    {1000003840, "jch", "Aliquam erat volutpat."},
    {1000003960, "Jean-Raphael", "Phasellus ac erat metus."},
    {1000004080, "jch", "Aenean pretium lectus at interdum fringilla."},
    {1000004200, "jch", "Sed fermentum aliquam nisi ut scelerisque."},
    {1000004320, "Jean-Raphael", "Fusce tincidunt scelerisque orci sed volutpat."},
    {1000004440, "jby", "Proin id euismod libero, in commodo mi."},
    {1000004560, "Jean-Raphael", "Nullam quis mi iaculis, vulputate diam nec, laoreet dolor."},
    {1000004680, "jby", "Morbi diam quam, dignissim et maximus id, placerat non dui."},
    {1000004800, "jch", "Suspendisse ultricies scelerisque vestibulum."},
    {1000004920, "jby", "Fusce ullamcorper arcu tincidunt nibh volutpat bibendum."},
    {1000005040, "Jean-Raphael", "Nunc pellentesque tellus sit amet dolor pretium auctor."},
    {1000005160, "jby", "Morbi quis felis ut sapien bibendum venenatis eu sed erat."},
    {1000005280, "Jean-Raphael", "Aliquam viverra elit ut quam ullamcorper, eget euismod diam facilisis."},
    {1000005400, "Jean-Raphael", "Etiam consectetur diam efficitur risus dictum ultrices."},
    {1000005520, "jby", "Donec vel sapien mollis, venenatis felis sit amet, dignissim dui."},
    {1000005640, "jby", "Nulla laoreet ligula non nibh scelerisque, eget vehicula odio finibus."},
    {1000005760, "Antonin", "Praesent at luctus augue, eu blandit nunc."},
    {1000005880, "jby", "Nam id mauris odio."},
    {1000006000, "jch", "Phasellus laoreet ex a tincidunt luctus."},
    {1000006120, "Antonin", "Quisque congue bibendum faucibus."},
    {1000006240, "Antonin", "Curabitur vulputate libero convallis leo ultricies lacinia."},
    {1000006360, "jch", "Duis sollicitudin, lorem et convallis commodo, elit leo congue urna, eget faucibus enim lorem ut nulla."},
    {1000006480, "jby", "Cras tortor massa, semper sed auctor vitae, pharetra a turpis."},
    {1000006600, "Jean-Raphael", "Donec placerat interdum lectus at eleifend."},
    {1000006720, "Antonin", "Morbi sagittis, arcu sed euismod sodales, tortor dui rutrum lectus, in imperdiet nisi massa a diam."},
    {1000006840, "Antonin", "Ut aliquet augue quis leo varius, ac convallis risus tristique."},
    {1000006960, "Jean-Raphael", "Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus."},
    {1000007080, "Antonin", "Morbi ultrices luctus posuere."},
    {1000007200, "Antonin", "Nunc id felis dictum, venenatis augue quis, ornare justo."},
    {1000007320, "jby", "Nulla sodales consequat pulvinar."},
    {1000007440, "jby", "Duis viverra augue ac arcu cursus, rhoncus rutrum tellus posuere."},
    {1000007560, "jby", "Integer fermentum bibendum elit a commodo."},
    {1000007680, "Jean-Raphael", "Phasellus eget erat laoreet, suscipit magna sit amet, pretium libero."},
    {1000007800, "jch", "Vestibulum ultricies ex efficitur elit viverra lacinia."},
    {1000007920, "Jean-Raphael", "Donec dignissim non massa ac porta."},
    {1000008040, "jby", "Integer rutrum, eros at pretium imperdiet, felis ante malesuada diam, vel vestibulum dolor augue et diam."},
    {1000008160, "jch", "Donec scelerisque, nisi at molestie pellentesque, purus risus blandit purus, id lobortis est felis ac tortor."},
    {1000008280, "jch", "In mattis vitae massa vel convallis."},
    {1000008400, "jch", "Aliquam congue bibendum ipsum sed tempus."},
    {1000008520, "Antonin", "Nullam posuere at augue id molestie."},
    {1000008640, "Jean-Raphael", "In sit amet elit venenatis odio vestibulum tincidunt."},
    {1000008760, "jby", "Nulla mauris sapien, faucibus non elit et, lobortis aliquet sapien."},
    {1000008880, "jby", "Nullam feugiat vitae turpis eu sollicitudin."},
    {1000009000, "jch", "Proin interdum molestie orci sed suscipit."},
    {1000009120, "Jean-Raphael", "Duis placerat felis vitae sapien fermentum volutpat."},
    {1000009240, "jch", "Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Quisque enim diam, sollicitudin non consectetur sit amet, sollicitudin vitae quam."},
    {1000009360, "jch", "Aenean hendrerit, nisi in feugiat tincidunt, nunc leo iaculis ipsum, semper pellentesque erat libero convallis orci."},
    {1000009480, "Jean-Raphael", "Integer feugiat malesuada pretium."},
    {1000009600, "jch", "Duis sit amet neque neque."},
    {1000009720, "Antonin", "Curabitur et fermentum lacus."},
    {1000009840, "Jean-Raphael", "Sed rutrum elementum quam nec laoreet."},
    {1000009960, "jby", "Mauris et eros at ligula efficitur aliquet."},
    {1000010080, "jby", "Suspendisse consectetur nulla eget tristique mollis."},
    {1000010200, "Jean-Raphael", "Nullam aliquam ante ut vehicula sagittis."},
    {1000010320, "Antonin", "Donec ac mauris sollicitudin, venenatis dui id, posuere ligula."},
    {1000010440, "jby", "Fusce ut mi quis mi volutpat interdum eget interdum velit."},
    {1000010560, "Jean-Raphael", "Sed vitae pretium magna, quis ullamcorper erat."},
    {1000010680, "jch", "Phasellus eleifend magna vitae odio porta egestas."},
    {1000010800, "jby", "Pellentesque porttitor orci turpis, sit amet eleifend nisi malesuada vel."},
    {1000010920, "jch", "Pellentesque quis diam nunc."},
    {1000011040, "jch", "Suspendisse aliquet pulvinar lectus."},
    {1000011160, "jby", "Morbi semper facilisis nisl sed tempor."},
    {1000011280, "Jean-Raphael", "Vivamus eu finibus sapien."},
    {1000011400, "jby", "Sed porttitor magna congue lacus vestibulum accumsan sed nec arcu."},
    {1000011520, "Jean-Raphael", "In hac habitasse platea dictumst."},
    {1000011640, "jby", "Nulla tempor sed massa vehicula bibendum."},
    {1000011760, "jch", "Quisque vitae ipsum ultricies, viverra quam non, porta enim."},
    {1000011880, "jby", "Nullam ac dignissim dolor."},
    {1000012000, "Jean-Raphael", "Nulla non ligula ut neque ullamcorper scelerisque."},
    {1000012120, "Antonin", "Nam sit amet mauris neque."},
    {1000012240, "jby", "Suspendisse iaculis erat eu orci volutpat hendrerit."},
    {1000012360, "jby", "In et nulla ut eros faucibus semper."},
    {1000012480, "Antonin", "In imperdiet nibh in consectetur tempus."},
    {1000012600, "jby", "Morbi purus mi, viverra ut libero eu, pellentesque bibendum lectus."},
    {1000012720, "jch", "Fusce a purus luctus, porttitor massa in, accumsan ex."},
    {1000012840, "Jean-Raphael", "Maecenas pretium tortor vel faucibus sodales."},
    {1000012960, "jch", "Cras a varius tortor."},
    {1000013080, "Jean-Raphael", "Aenean sem nisl, rhoncus id eros sit amet, gravida vehicula felis."},
    {1000013200, "jby", "Maecenas ultrices condimentum risus id blandit."},
    {1000013320, "Jean-Raphael", "Nullam orci nisl, pretium at risus eu, sodales facilisis neque."},
    {1000013440, "Jean-Raphael", "Sed consequat eros a suscipit vehicula."},
    {1000013560, "Antonin", "Praesent ut turpis vitae sem congue accumsan a et felis."},
    {1000013680, "Antonin", "Vestibulum at lectus vel lectus iaculis mattis."},
    {1000013800, "Jean-Raphael", "In ut faucibus orci."},
    {1000013920, "jch", "Sed lobortis lorem nec erat gravida, eget consectetur velit viverra."},
};

static char *trim(char *str)
{
    while (isspace((unsigned char)*str)) ++str;
    if (*str == '\0')
        return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) --end;
    *(end+1) = 0;
    return str;
}

static void usage(void) {
    fputs("Usage: tchatche [server_pipe]\n", stderr);
    exit(EXIT_FAILURE);
}

static void options_handler(int argc, char *argv[], char **server_path) {
    opterr = 0;
    int hflag = 0, vflag = 0, c;
    while ((c = getopt(argc, argv, "hv")) != -1) {
        switch (c) {
        case 'h': hflag = 1; break;
        case 'v': vflag = 1; break;
        case '?':
            if (isprint(optopt))
                fprintf(stderr, "Unknown option '-%c'.\n", optopt);
            else
                fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
            usage();
        default:
            usage();
        }
    }

    if (vflag) {
        puts("tchatche dev version\n"
             "MIT License - "
             "Copyright (c) 2016 Antonin Décimo, Jean-Raphaël Gaglione");
        exit(EXIT_SUCCESS);
    } else if (hflag) {
        puts("Usage: tchatche [server_pipe]\n"
             "\t-h\thelp\n"
             "\t-v\tversion");
        exit(EXIT_SUCCESS);
    }

    if (optind == argc)
        *server_path = "/tmp/tchatche/server";
    else if (optind == argc - 1)
        *server_path = argv[optind];
    else
        usage();
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    int server_pipe, client_pipe;
    char *server_path, *client_path;

    options_handler(argc, argv, &server_path);

    /* Open pipes */
    if ((server_pipe = open(server_path, O_WRONLY)) == -1)
        error_exit(server_path);
    client_path = mktmpfifo_client();
    if ((client_pipe = open(client_path, O_RDONLY | O_NONBLOCK)) == -1) {
        perror(client_path);
        unlink(client_path);
        free(client_path);
        close(server_pipe);
        return EXIT_FAILURE;
    }

    /* Init curses */
    tui_init_curses();
    tui *ui = tui_init();
    tui_print_info(ui, 0);
    tui_refresh(ui);

    /* Event loop */
    size_t current_msg = 0;
    char buffer[9999] = {0};
    int ch, r;
    while ((ch = wgetch(ui->input)) != TUI_QUIT) {

        /* Messages handler */
        if ((r = read(client_pipe, buffer, sizeof(buffer) - 1))) {
            buffer[r] = '\0';
            tui_add_txt(ui, buffer);
            tui_refresh(ui);
        }

        /* Input handler */
        if (ch == ERR) continue;
        tui_print_info(ui, ch);
        tui_refresh(ui);
        switch (ch) {
        case KEY_UP:
            if (ui->chat_row > 0) {
                --(ui->chat_row);
                tui_refresh(ui);
            }
            break;
        case KEY_DOWN:
            if (ui->chat_row < getmaxy(ui->chat) - getmaxy(stdscr) - 1) {
                ++(ui->chat_row);
                tui_refresh(ui);
            }
            break;
        case KEY_LEFT:
            form_driver(ui->form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(ui->form, REQ_NEXT_CHAR);
            break;
        case '\177': /* DEL */
            form_driver(ui->form, REQ_DEL_PREV);
            if (data_behind(ui->form)) {
                if (getcurx(ui->input) > 0) {
                    form_driver(ui->form, REQ_SCR_BCHAR);
                }
                form_driver(ui->form, REQ_NEXT_CHAR);
            } else if (getcurx(ui->input) == 0) {
                form_driver(ui->form, REQ_DEL_CHAR);
            }
            break;
        case '\r':
        case '\n': {
            form_driver(ui->form, REQ_VALIDATION);
            char *buf = trim(field_buffer(ui->fields[0], 0));
            const size_t len = strlen(buf);
            if (len > 0) {
                write(server_pipe, buf, len + 1);
                tui_add_txt(ui, buf);
                form_driver(ui->form, REQ_CLR_FIELD);
                tui_refresh(ui);
            }
            break;
        }
        case KEY_F(2):
            tui_add_msg(ui, messages + (current_msg++) % array_size(messages));
            tui_print_info(ui, ch);
            tui_refresh(ui);
            break;
        default:
            form_driver(ui->form, ch);
            break;
        }
    }

    close(server_pipe);
    close(client_pipe);
    unlink(client_path);
    free(client_path);
    tui_end(ui);
    tui_end_curses();
    return EXIT_SUCCESS;
}
