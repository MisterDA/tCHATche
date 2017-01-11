#include "process.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "client.h"
#include "request.h"


static void
send_file(transfer *t)
{
	tui_print_txt(cl->ui, "Begin transfer of \"%s\"...", t->filename);
	FILE *file = fopen(t->filename, "r");
	size_t r;
	char buf[256];
	while ((r = fread(buf, 1, 256u, file))) {
		writedata(cl->server_pipe,
			req_client_FILE_transfer(++(t->series), t->id, mem2data(NULL, buf, r)));
	}
	if (feof(file)) {
		tui_add_txt(cl->ui, "The file was successfully transmitted.");
	} else if (ferror(file)) {
		tui_print_txt(cl->ui, "Read error while transmitting %s.", t->filename);
	}
	fclose(file);
	destroy_transfer(t);
}

int
pro_server_OKOK(uint32_t id)
{
	if (!cl->has_id) { /* connection */
		tui_print_txt(cl->ui, "Nick has been set to <%s>.\n", cl->nick);
		cl->has_id = true;
		cl->id = id;
		free(cl->ui->title);
		asprintf(&cl->ui->title, "tCHATche (%s)", cl->nick);
        tui_print_info(cl->ui, 0);
        tui_refresh(cl->ui);
	} else { /* file transfer */
		cl->upload->id = id;
		send_file(cl->upload);
	}
	return 0;
}

int
pro_server_BADD()
{
	if (!cl->has_id) { /* connection */
		tui_add_txt(cl->ui, "Invalid nickname.\n");
	} else { /* file transfer */
		tui_add_txt(cl->ui, "The file tranfer was rejected.");
		destroy_transfer(cl->upload);
	}
	return 0;
}

int
pro_server_BYEE(uint32_t id)
{
	if (cl->id != id)
		return -1;
	client_end(cl);
	exit(EXIT_SUCCESS);
	return 0;
}

int
pro_server_BCST(char *nick, char *msg, size_t msglen)
{
	char *message = strndup(msg, msglen);
	tui_add_msg(cl->ui, &(tui_msg){time(NULL), nick, message});
	free(message);
	return 0;
}

int
pro_server_PRVT(char *nick, char *msg, size_t msglen)
{
	char *message = strndup(msg, msglen);
	tui_add_prvt_msg(cl->ui, &(tui_msg){time(NULL), nick, message}, false);
	free(message);
	return 0;
}

int
pro_server_LIST(uint32_t n, char *nick)
{
	tui_add_user(cl->ui, n, nick);
	return 0;
}

int
pro_server_SHUT(char *nick)
{
	tui_print_txt(cl->ui, "<%s> has stopped the server !\n", nick);
	client_end(cl);
	exit(EXIT_SUCCESS);
	return 0;
}

int
pro_server_FILE_announce(uint32_t intransfert, uint32_t len, char *filename,
	char *nick)
{
	if (nick)
		tui_print_txt(cl->ui, "Receiving file \"%s\" [%u] from <%s>...",
			filename, len, nick);
	else
		tui_print_txt(cl->ui, "Receiving file \"%s\" [%u]...", filename, len);

	transfer *t = malloc(sizeof(*t));
	t->id = intransfert;
	t->series = 0;
	t->len = len;
	t->filename = strdup(filename);
	t->nick = nick ? strdup(nick) : NULL;

	if (!access(t->filename, F_OK)) {
		tui_add_txt(cl->ui, "Refused: file already exists.");
	} else if (!(t->out = fopen(t->filename, "w"))) {
		tui_print_txt(cl->ui, "Refused: %s", strerror(errno));
	} else {
		arlist_add(cl->downloads, compare_transfers, t);
		return 0;
	}
	destroy_transfer(t);
	return -1;
}

int
pro_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf)
{
	transfer *t = transfer_from_id(cl->downloads, idtransfer);
	if (!t) return -1;
	if (t->out == NULL) return -1;
	if (t->series != serie -1) {
		tui_print_txt(cl->ui, "Wrong series for %u, expected %u got %u.",
			t->id, t->series + 1, serie);
	}
	if (fwrite(buf.ata, 1, buf.length, t->out) != buf.length) {
		tui_print_txt(cl->ui, "Error while writing %u : %s", t->id, strerror(errno));
		return -1;
	}
	fflush(t->out);
	if (++(t->series) * 256 >= t->len) {
		tui_print_txt(cl->ui, "End of \"%s\" transfer.", t->filename);
		fclose(t->out);
		destroy_transfer(t);
		arlist_remove(cl->downloads, index_of(cl->downloads, compare_transfers, t));
	}
	return 0;
}
