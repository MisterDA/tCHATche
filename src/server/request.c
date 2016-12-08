#include "request.h"

/* outils de chantier: (reposer après utilisation) //

printf("HERE\n"); fflush(stdout);

//*/

static char * // pas static, pas ici
malloc_request(data *d, type t, size_t length) {
    char *wr = malloc_data(d, SIZEOF_REQUEST(length));
    if (!wr) goto err0;
    if (!write_num(&wr, d->length)) goto err1;
    if (!write_type(&wr, t)) goto err1;
    return wr;
    err1: empty_data(d);
    err0: return NULL;
}


data
req_server_OKOK(uint32_t id) {
    data d;
    char *wr = malloc_request(&d,"OKOK",SIZEOF_NUM);
    if (!wr) goto err0;
    if (!write_num(&wr, id)) goto err1;
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_BADD() {
    data d;
    char *wr = malloc_request(&d,"BADD",0);
    if (!wr) goto err0;
    return d;
    //err1: empty_data(&d);
    err0: return d;
}

data
req_server_BYEE(uint32_t id) {
    data d;
    char *wr = malloc_request(&d,"BYEE",SIZEOF_NUM);
    if (!wr) goto err0;
    if (!write_num(&wr, id)) goto err1;
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_BCST(char *pseudo, char *msg, size_t msglen) {
    data d;
    size_t pslen = strlen(pseudo);
    char *wr = malloc_request(&d,"BCST",SIZEOF_STR(pslen)+SIZEOF_STR(msglen));
    if (!wr) goto err0;
    if (!write_str(&wr, pseudo, pslen)) goto err1;
    if (!write_str(&wr, msg, msglen)) goto err1;
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_PRVT(char *pseudo, char *msg, size_t msglen) {
    data d;
    size_t pslen = strlen(pseudo);
    char *wr = malloc_request(&d,"PRVT",SIZEOF_STR(pslen)+SIZEOF_STR(msglen));
    if (!wr) goto err0;
    if (!write_str(&wr, pseudo, pslen)) goto err1;
    if (!write_str(&wr, msg, msglen)) goto err1;
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_LIST(uint32_t n, char *pseudo) {
    data d;
    size_t pslen = strlen(pseudo);
    char *wr = malloc_request(&d,"LIST",SIZEOF_NUM+SIZEOF_STR(pslen));
    if (!wr) goto err0;
    if (!write_num(&wr, n)) goto err1;
    if (!write_str(&wr, pseudo, pslen)) goto err1;
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_SHUT(char *pseudo) {
    data d;
    size_t pslen = strlen(pseudo);
    char *wr = malloc_request(&d,"SHUT",SIZEOF_STR(pslen));
    if (!wr) goto err0;
    if (!write_str(&wr, pseudo, pslen)) goto err1;
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_FILE_announce(uint32_t idtransfer, uint32_t len, char *filename, char *pseudo) {
    data d;
    size_t fnlen = strlen(filename);
    size_t pslen = strlen(pseudo);
    char *wr = malloc_request(&d,"FILE",SIZEOF_NUM+SIZEOF_NUM+SIZEOF_LONGNUM+SIZEOF_STR(fnlen)+SIZEOF_STR(pslen));
    if (!wr) goto err0;
    if (!write_num(&wr, 0)) goto err1;
    if (!write_num(&wr, idtransfer)) goto err1;
    if (!write_longnum(&wr, len)) goto err1;
    if (!write_str(&wr, filename, fnlen)) goto err1;
    if (!write_str(&wr, pseudo, pslen)) goto err1; // extention du protocole : pseudo de l'expediteur
    return d;
    err1: empty_data(&d);
    err0: return d;
}

data
req_server_FILE_transfer(uint32_t serie, uint32_t idtransfer, data buf) {
    data d;
    if (serie==0) goto err1;
    char *wr = malloc_request(&d,"FILE",SIZEOF_NUM+SIZEOF_NUM+SIZEOF_DATA(buf));
    if (!wr) goto err0;
    if (!write_num(&wr, serie)) goto err1;
    if (!write_num(&wr, idtransfer)) goto err1;
    //TODO: write_data(&wr, buf)
    return d;
    err1: empty_data(&d);
    err0: return d;
}
