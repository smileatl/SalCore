/*
* FreeBASE Modular Media Switching Software Library / Soft-Switch Application
* Copyright (C) 2005-2014, Anthony Minessale II <anthm@freebase.org>
*
* Version: MPL 1.1
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is FreeBASE Modular Media Switching Software Library / Soft-Switch Application
*
* The Initial Developer of the Original Code is
* Anthony Minessale II <anthm@freebase.org>
* Portions created by the Initial Developer are Copyright (C)
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Anthony Minessale II <anthm@freebase.org>
* Simon Capper <skyjunky@sbcglobal.net>
* Marc Olivier Chouinard <mochouinard@moctel.com>
* Raymond Chandler <intralanman@freebase.org>
*
* base_xml.c -- XML PARSER
*
* Derived from ezxml http://ezxml.sourceforge.net
* Original Copyright
*
* Copyright 2004, 2006 Aaron Voisine <aaron@voisine.org>
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <base.h>
#ifndef WIN32
#include <sys/wait.h>
#include <base_private.h>
#include <glob.h>
#else /* we're on windoze :( */
/* glob functions at end of this file */
#include <fspr_file_io.h>

typedef struct {
    size_t gl_pathc;			/* Count of total paths so far. */
    size_t gl_matchc;			/* Count of paths matching pattern. */
    size_t gl_offs;				/* Reserved at beginning of gl_pathv. */
    int gl_flags;				/* Copy of flags parameter to glob. */
    char **gl_pathv;			/* List of paths matching pattern. */
                                /* Copy of errfunc parameter to glob. */
    int (*gl_errfunc) (const char *, int);
} glob_t;

/* Believed to have been introduced in 1003.2-1992 */
#define	GLOB_APPEND		0x0001	/* Append to output from previous call. */
#define	GLOB_DOOFFS		0x0002	/* Use gl_offs. */
#define	GLOB_ERR		0x0004	/* Return on error. */
#define	GLOB_MARK		0x0008	/* Append / to matching directories. */
#define	GLOB_NOCHECK	0x0010	/* Return pattern itself if nothing matches. */
#define	GLOB_NOSORT		0x0020	/* Don't sort. */

/* Error values returned by glob(3) */
#define	GLOB_NOSPACE	(-1)	/* Malloc call failed. */
#define	GLOB_ABORTED	(-2)	/* Unignored error. */
#define	GLOB_NOMATCH	(-3)	/* No match and GLOB_NOCHECK was not set. */
#define	GLOB_NOSYS		(-4)	/* Obsolete: source comptability only. */

#define	GLOB_ALTDIRFUNC	0x0040	/* Use alternately specified directory funcs. */
#define	GLOB_MAGCHAR	0x0100	/* Pattern had globbing characters. */
#define	GLOB_NOMAGIC	0x0200	/* GLOB_NOCHECK without magic chars (csh). */
#define	GLOB_QUOTE		0x0400	/* Quote special chars with \. */
#define	GLOB_LIMIT		0x1000	/* limit number of returned paths */

int glob(const char *, int, int (*)(const char *, int), glob_t *);
void globfree(glob_t *);

#endif

#define BASE_XML_WS   "\t\r\n "	/* whitespace */
#define BASE_XML_ERRL 128		/* maximum error string length */

static void preprocess_exec_set(char *keyval)
{
    char *key = keyval;
    char *val = strchr(keyval, '=');

    if (val) {
        char *ve = val++;
        while (*val && *val == ' ') {
            val++;
        }
        *ve-- = '\0';
        while (*ve && *ve == ' ') {
            *ve-- = '\0';
        }
    }

    if (key && val) {
        base_stream_handle_t exec_result = { 0 };
        BASE_STANDARD_STREAM(exec_result);
        if (base_stream_system(val, &exec_result) == 0) {
            if (!zstr(exec_result.data)) {
                char *tmp = (char *) exec_result.data;
                tmp = &tmp[strlen(tmp)-1];
                while (tmp >= (char *) exec_result.data && ( tmp[0] == ' ' || tmp[0] == '\n') ) {
                    tmp[0] = '\0'; /* remove trailing spaces and newlines */
                    tmp--;
                }
                base_core_set_variable(key, exec_result.data);
            }
        } else {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Error while executing command: %s\n", val);
        }
        base_safe_free(exec_result.data);
    }
}

static void preprocess_stun_set(char *keyval)
{
    char *key = keyval;
    char *val = strchr(keyval, '=');

    if (val) {
        char *ve = val++;
        while (*val && *val == ' ') {
            val++;
        }
        *ve-- = '\0';
        while (*ve && *ve == ' ') {
            *ve-- = '\0';
        }
    }

    if (key && val) {
        char *external_ip = NULL;
        base_memory_pool_t *pool;

        base_core_new_memory_pool(&pool);

        if (base_stun_ip_lookup(&external_ip, val, pool) == BASE_STATUS_SUCCESS) {
            if (!zstr(external_ip)) {
                char *tmp = external_ip;
                tmp = &tmp[strlen(tmp) - 1];
                while (tmp >= external_ip && (tmp[0] == ' ' || tmp[0] == '\n')) {
                    tmp[0] = '\0'; /* remove trailing spaces and newlines */
                    tmp--;
                }
                base_core_set_variable(key, external_ip);
            }
        } else {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "stun-set failed.\n");
        }

        base_core_destroy_memory_pool(&pool);
    }
}

static void preprocess_env_set(char *keyval)
{
    char *key = keyval;
    char *val = strchr(keyval, '=');

    if (key && val) {
        *val++ = '\0';

        if (*val++ == '$') {
            char *data = getenv(val);

            if (data) {
                base_core_set_variable(key, data);
            }
        }
    }
}

static int preprocess(const char *cwd, const char *file, FILE *write_fd, int rlevel);

typedef struct base_xml_root *base_xml_root_t;
struct base_xml_root {		/* additional data for the root tag */
    struct base_xml xml;		/* is a super-struct built on top of base_xml struct */
    base_xml_t cur;			/* current xml tree insertion point */
    char *m;					/* original xml string */
    base_size_t len;			/* length of allocated memory */
    uint8_t dynamic;			/* Free the original string when calling base_xml_free */
    char *u;					/* UTF-8 conversion of string if original was UTF-16 */
    char *s;					/* start of work area */
    char *e;					/* end of work area */
    char **ent;					/* general entities (ampersand sequences) */
    char ***attr;				/* default attributes */
    char ***pi;					/* processing instructions */
    short standalone;			/* non-zero if <?xml standalone="yes"?> */
    char err[BASE_XML_ERRL];	/* error string */
};

char *BASE_XML_NIL[] = { NULL };	/* empty, null terminated array of strings */

struct base_xml_binding {
    base_xml_search_function_t function;
    base_xml_section_t sections;
    void *user_data;
    struct base_xml_binding *next;
};


static base_xml_binding_t *BINDINGS = NULL;
static base_xml_t MAIN_XML_ROOT = NULL;
static base_memory_pool_t *XML_MEMORY_POOL = NULL;

static base_thread_rwlock_t *B_RWLOCK = NULL;
static base_mutex_t *XML_LOCK = NULL;
static base_mutex_t *CACHE_MUTEX = NULL;
static base_mutex_t *REFLOCK = NULL;
static base_mutex_t *FILE_LOCK = NULL;

BASE_DECLARE_NONSTD(base_xml_t) __base_xml_open_root(uint8_t reload, const char **err, void *user_data);

static base_xml_open_root_function_t XML_OPEN_ROOT_FUNCTION = (base_xml_open_root_function_t)__base_xml_open_root;
static void *XML_OPEN_ROOT_FUNCTION_USER_DATA = NULL;

static base_hash_t *CACHE_HASH = NULL;
static base_hash_t *CACHE_EXPIRES_HASH = NULL;

struct xml_section_t {
    const char *name;
    /* base_xml_section_t section; */
    uint32_t section;
};

static struct xml_section_t SECTIONS[] = {
    {"result", BASE_XML_SECTION_RESULT},
    {"config", BASE_XML_SECTION_CONFIG},
    {"directory", BASE_XML_SECTION_DIRECTORY},
    {"dialplan", BASE_XML_SECTION_DIALPLAN},
    {"languages", BASE_XML_SECTION_LANGUAGES},
    {"chatplan", BASE_XML_SECTION_CHATPLAN},
    {"channels", BASE_XML_SECTION_CHANNELS},
    {NULL, 0}
};

BASE_DECLARE(base_xml_section_t) base_xml_parse_section_string(const char *str)
{
    size_t x;
    char buf[1024] = "";
    /*base_xml_section_t sections = BASE_XML_SECTION_RESULT; */
    uint32_t sections = BASE_XML_SECTION_RESULT;

    if (str) {
        for (x = 0; x < strlen(str); x++) {
            buf[x] = (char) tolower((int) str[x]);
        }
        for (x = 0;; x++) {
            if (!SECTIONS[x].name) {
                break;
            }
            if (strstr(buf, SECTIONS[x].name)) {
                sections |= SECTIONS[x].section;
            }
        }
    }
    return (base_xml_section_t) sections;
}

BASE_DECLARE(base_status_t) base_xml_unbind_search_function(base_xml_binding_t **binding)
{
    base_xml_binding_t *ptr, *last = NULL;
    base_status_t status = BASE_STATUS_FALSE;


    base_thread_rwlock_wrlock(B_RWLOCK);
    for (ptr = BINDINGS; ptr; ptr = ptr->next) {
        if (ptr == *binding) {
            if (last) {
                last->next = (*binding)->next;
            } else {
                BINDINGS = (*binding)->next;
            }
            status = BASE_STATUS_SUCCESS;
            break;
        }
        last = ptr;
    }
    base_thread_rwlock_unlock(B_RWLOCK);

    return status;
}

BASE_DECLARE(base_status_t) base_xml_unbind_search_function_ptr(base_xml_search_function_t function)
{
    base_xml_binding_t *ptr, *last = NULL;
    base_status_t status = BASE_STATUS_FALSE;

    base_thread_rwlock_wrlock(B_RWLOCK);
    for (ptr = BINDINGS; ptr; ptr = ptr->next) {
        if (ptr->function == function) {
            status = BASE_STATUS_SUCCESS;

            if (last) {
                last->next = ptr->next;
            } else {
                BINDINGS = ptr->next;
                last = NULL;
                continue;
            }
        }
        last = ptr;
    }
    base_thread_rwlock_unlock(B_RWLOCK);

    return status;
}

BASE_DECLARE(void) base_xml_set_binding_sections(base_xml_binding_t *binding, base_xml_section_t sections)
{
    base_assert(binding);
    binding->sections = sections;
}

BASE_DECLARE(void) base_xml_set_binding_user_data(base_xml_binding_t *binding, void *user_data)
{
    base_assert(binding);
    binding->user_data = user_data;
}

BASE_DECLARE(base_xml_section_t) base_xml_get_binding_sections(base_xml_binding_t *binding)
{
    return binding->sections;
}

BASE_DECLARE(void *) base_xml_get_binding_user_data(base_xml_binding_t *binding)
{
    return binding->user_data;
}

BASE_DECLARE(base_status_t) base_xml_bind_search_function_ret(base_xml_search_function_t function,
    base_xml_section_t sections, void *user_data, base_xml_binding_t **ret_binding)
{
    base_xml_binding_t *binding = NULL, *ptr = NULL;
    assert(function != NULL);

    if (!(binding = (base_xml_binding_t *) base_core_alloc(XML_MEMORY_POOL, sizeof(*binding)))) {
        return BASE_STATUS_MEMERR;
    }

    binding->function = function;
    binding->sections = sections;
    binding->user_data = user_data;

    base_thread_rwlock_wrlock(B_RWLOCK);
    for (ptr = BINDINGS; ptr && ptr->next; ptr = ptr->next);

    if (ptr) {
        ptr->next = binding;
    } else {
        BINDINGS = binding;
    }

    if (ret_binding) {
        *ret_binding = binding;
    }

    base_thread_rwlock_unlock(B_RWLOCK);

    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(base_xml_t) base_xml_find_child(base_xml_t node, const char *childname, const char *attrname, const char *value)
{
    base_xml_t p = NULL;

    if (!(childname && attrname && value)) {
        return node;
    }

    for (p = base_xml_child(node, childname); p; p = p->next) {
        const char *aname = base_xml_attr(p, attrname);
        if (aname && !strcasecmp(aname, value)) {
            break;
        }
    }

    return p;
}

BASE_DECLARE(base_xml_t) base_xml_find_child_multi(base_xml_t node, const char *childname,...)
{
    base_xml_t p = NULL;
    const char *names[256] = { 0 };
    const char *vals[256] = { 0 };
    int x, i = 0;
    va_list ap;
    const char *attrname, *value = NULL;

    va_start(ap, childname);

    while (i < 255) {
        if ((attrname = va_arg(ap, const char *))) {
            value = va_arg(ap, const char *);
        }
        if (attrname && value) {
            names[i] = attrname;
            vals[i] = value;
        } else {
            break;
        }
        i++;
    }

    va_end(ap);

    if (!(childname && i)) {
        return node;
    }

    for (p = base_xml_child(node, childname); p; p = p->next) {
        for (x = 0; x < i; x++) {
            if (names[x] && vals[x]) {
                const char *aname = base_xml_attr(p, names[x]);

                if (aname) {
                    if (*vals[x] == '!') {
                        const char *sval = vals[x] + 1;
                        if (strcasecmp(aname, sval)) {
                            goto done;
                        }
                    } else {
                        if (!strcasecmp(aname, vals[x])) {
                            goto done;
                        }
                    }
                }
            }
        }
    }

done:

    return p;
}

/* returns the first child tag with the given name or NULL if not found */
BASE_DECLARE(base_xml_t) base_xml_child(base_xml_t xml, const char *name)
{
    xml = (xml) ? xml->child : NULL;
    while (xml && strcmp(name, xml->name))
        xml = xml->sibling;
    return xml;
}

/* returns the Nth tag with the same name in the same subsection or NULL if not found */
base_xml_t base_xml_idx(base_xml_t xml, int idx)
{
    for (; xml && idx; idx--)
        xml = xml->next;
    return xml;
}

/* returns the value of the requested tag attribute or "" if not found */
BASE_DECLARE(const char *) base_xml_attr_soft(base_xml_t xml, const char *attr)
{
    const char *ret = base_xml_attr(xml, attr);

    return ret ? ret : "";
}

/* returns the value of the requested tag attribute or NULL if not found */
BASE_DECLARE(const char *) base_xml_attr(base_xml_t xml, const char *attr)
{
    int i = 0, j = 1;
    base_xml_root_t root = (base_xml_root_t) xml;

    if (!xml || !xml->attr)
        return NULL;
    while (xml->attr[i] && attr && strcmp(attr, xml->attr[i]))
        i += 2;
    if (xml->attr[i])
        return xml->attr[i + 1];	/* found attribute */

    while (root->xml.parent)
        root = (base_xml_root_t) root->xml.parent;	/* root tag */

    if (!root->attr) {
        return NULL;
    }

    for (i = 0; root->attr[i] && xml->name && strcmp(xml->name, root->attr[i][0]); i++);
    if (!root->attr[i])
        return NULL;			/* no matching default attributes */
    while (root->attr[i][j] && attr && strcmp(attr, root->attr[i][j]))
        j += 3;
    return (root->attr[i][j]) ? root->attr[i][j + 1] : NULL;	/* found default */
}

/* same as base_xml_get but takes an already initialized va_list */
static base_xml_t base_xml_vget(base_xml_t xml, va_list ap)
{
    char *name = va_arg(ap, char *);
    int idx = -1;

    if (name && *name) {
        idx = va_arg(ap, int);
        xml = base_xml_child(xml, name);
    }
    return (idx < 0) ? xml : base_xml_vget(base_xml_idx(xml, idx), ap);
}

/* Traverses the xml tree to retrieve a specific subtag. Takes a variable
length list of tag names and indexes. The argument list must be terminated
by either an index of -1 or an empty string tag name. Example:
title = base_xml_get(library, "shelf", 0, "book", 2, "title", -1);
This retrieves the title of the 3rd book on the 1st shelf of library.
Returns NULL if not found. */
BASE_DECLARE(base_xml_t) base_xml_get(base_xml_t xml,...)
{
    va_list ap;
    base_xml_t r;

    va_start(ap, xml);
    r = base_xml_vget(xml, ap);
    va_end(ap);
    return r;
}

/* returns a null terminated array of processing instructions for the given target */
BASE_DECLARE(const char **) base_xml_pi(base_xml_t xml, const char *target)
{
    base_xml_root_t root = (base_xml_root_t) xml;
    int i = 0;

    if (!root) {
        return (const char **) BASE_XML_NIL;
    }

    while (root && root->xml.parent) {
        root = (base_xml_root_t) root->xml.parent;	/* root tag */
    }

    if (!root || !root->pi) {
        return (const char **) BASE_XML_NIL;
    }

    while (root->pi[i] && strcmp(target, root->pi[i][0])) {
        i++;					/* find target */
    }

    return (const char **) ((root->pi[i]) ? root->pi[i] + 1 : BASE_XML_NIL);
}

/* set an error string and return root */
static base_xml_t base_xml_err(base_xml_root_t root, char *s, const char *err, ...)
{
    va_list ap;
    int line = 1;
    char *t, fmt[BASE_XML_ERRL];

    if (!root || !root->s) {
        return NULL;
    }

    for (t = root->s; t && t < s; t++)
        if (*t == '\n')
            line++;
    base_snprintf(fmt, BASE_XML_ERRL, "[error near line %d]: %s", line, err);

    va_start(ap, err);
    vsnprintf(root->err, BASE_XML_ERRL, fmt, ap);
    va_end(ap);

    return &root->xml;
}

/* Recursively decodes entity and character references and normalizes new lines
ent is a null terminated array of alternating entity names and values. set t
to '&' for general entity decoding, '%' for parameter entity decoding, 'c'
for cdata sections, ' ' for attribute normalization, or '*' for non-cdata
attribute normalization. Returns s, or if the decoded string is longer than
s, returns a malloced string that must be freed. */
static char *base_xml_decode(char *s, char **ent, char t)
{
    char *e, *r = s, *m = s;
    unsigned long b, c, d, l;

    for (; *s; s++) {			/* normalize line endings */
        while (*s == '\r') {
            *(s++) = '\n';
            if (*s == '\n')
                memmove(s, (s + 1), strlen(s));
        }
    }

    for (s = r;;) {
        while (*s && *s != '&' && (*s != '%' || t != '%') && !isspace((unsigned char) (*s)))
            s++;

        if (!*s)
            break;
        else if (t != 'c' && !strncmp(s, "&#", 2)) {	/* character reference */
            char *code = s + 2;
            int base = 10;
            if (*code == 'x') {
                code++;
                base = 16;
            }
            if (!isxdigit((int)*code)) { /* "&# 1;" and "&#-1;" are invalid */
                s++;
                continue;
            }
            c = strtoul(code, &e, base);
            if (!c || *e != ';') {
                s++;
                continue;
            }
            /* not a character ref */
            if (c < 0x80)
                *(s++) = (char) c;	/* US-ASCII subset */
            else if (c > 0x7FFFFFFF) { /* out of UTF-8 range */
                s++;
                continue;
            } else {				/* multi-byte UTF-8 sequence */
                for (b = 0, d = c; d; d /= 2)
                    b++;		/* number of bits in c */
                b = (b - 2) / 5;	/* number of bytes in payload */
                assert(b < 7);		/* because c <= 0x7FFFFFFF */
                *(s++) = (char) ((0xFF << (7 - b)) | (c >> (6 * b)));	/* head */
                while (b)
                    *(s++) = (char) (0x80 | ((c >> (6 * --b)) & 0x3F));	/* payload */
            }

            memmove(s, strchr(s, ';') + 1, strlen(strchr(s, ';')));
        } else if ((*s == '&' && (t == '&' || t == ' ' || t == '*')) || (*s == '%' && t == '%')) {	/* entity reference */
            for (b = 0; ent[b] && strncmp(s + 1, ent[b], strlen(ent[b])); b += 2);	/* find entity in entity list */

            if (ent[b++]) {		/* found a match */
                if ((c = (unsigned long) strlen(ent[b])) - 1 > (e = strchr(s, ';')) - s) {
                    l = (d = (unsigned long) (s - r)) + c + (unsigned long) strlen(e);	/* new length */
                    if (l) {
                        if (r == m) {
                            char *tmp = (char *) base_must_malloc(l);
                            r = strcpy(tmp, r);
                        } else {
                            r = (char *) base_must_realloc(r, l);
                        }
                    }
                    e = strchr((s = r + d), ';');	/* fix up pointers */
                }

                memmove(s + c, e + 1, strlen(e));	/* shift rest of string */
                strncpy(s, ent[b], c);	/* copy in replacement text */
            } else
                s++;			/* not a known entity */
        } else if ((t == ' ' || t == '*') && isspace((int) (*s)))
            *(s++) = ' ';
        else
            s++;				/* no decoding needed */
    }

    if (t == '*') {				/* normalize spaces for non-cdata attributes */
        for (s = r; *s; s++) {
            if ((l = (unsigned long) strspn(s, " ")))
                memmove(s, s + l, strlen(s + l) + 1);
            while (*s && *s != ' ')
                s++;
        }
        if (--s >= r && *s == ' ')
            *s = '\0';			/* trim any trailing space */
    }
    return r;
}

/* called when parser finds start of new tag */
static void base_xml_open_tag(base_xml_root_t root, char *name, char *open_pos, char **attr)
{
    base_xml_t xml;

    if (!root || !root->cur) {
        return;
    }

    xml = root->cur;

    if (xml->name)
        xml = base_xml_add_child(xml, name, strlen(xml->txt));
    else
        xml->name = name;		/* first open tag */

    xml->attr = attr;
    root->cur = xml;			/* update tag insertion point */
    root->cur->open = open_pos;
}

/* called when parser finds character content between open and closing tag */
static void base_xml_char_content(base_xml_root_t root, char *s, base_size_t len, char t)
{
    base_xml_t xml;
    char *m = s;
    base_size_t l;

    if (!root || !root->cur) {
        return;
    }

    xml = root->cur;

    if (!xml || !xml->name || !len)
        return;					/* sanity check */

    s[len] = '\0';				/* null terminate text (calling functions anticipate this) */
    len = strlen(s = base_xml_decode(s, root->ent, t)) + 1;

    if (!*(xml->txt))
        xml->txt = s;			/* initial character content */
    else {						/* allocate our own memory and make a copy */
        if ((xml->flags & BASE_XML_TXTM)) {	/* allocate some space */
            xml->txt = (char *) base_must_realloc(xml->txt, (l = strlen(xml->txt)) + len);
        } else {
            char *tmp = (char *) base_must_malloc((l = strlen(xml->txt)) + len);

            xml->txt = strcpy(tmp, xml->txt);
        }
        strcpy(xml->txt + l, s);	/* add new char content */
        if (s != m)
            free(s);			/* free s if it was malloced by base_xml_decode() */
    }

    if (xml->txt != m)
        base_xml_set_flag(xml, BASE_XML_TXTM);
}

/* called when parser finds closing tag */
static base_xml_t base_xml_close_tag(base_xml_root_t root, char *name, char *s, char *close_pos)
{
    if (!root || !root->cur || !root->cur->name || strcmp(name, root->cur->name))
        return base_xml_err(root, s, "unexpected closing tag </%s>", name);

    root->cur->close = close_pos;
    root->cur = root->cur->parent;
    return NULL;
}

/* checks for circular entity references, returns non-zero if no circular
references are found, zero otherwise */
static int base_xml_ent_ok(char *name, char *s, char **ent)
{
    int i;

    for (;; s++) {
        while (*s && *s != '&')
            s++;				/* find next entity reference */
        if (!*s)
            return 1;
        if (!strncmp(s + 1, name, strlen(name)))
            return 0;			/* circular ref. */
        for (i = 0; ent[i] && strncmp(ent[i], s + 1, strlen(ent[i])); i += 2);
        if (ent[i] && !base_xml_ent_ok(name, ent[i + 1], ent))
            return 0;
    }
}

/* called when the parser finds a processing instruction */
static void base_xml_proc_inst(base_xml_root_t root, char *s, base_size_t len)
{
    int i = 0, j = 1;
    char *target = s;
    char **sstmp;
    char *stmp;

    s[len] = '\0';				/* null terminate instruction */
    if (*(s += strcspn(s, BASE_XML_WS))) {
        *s = '\0';				/* null terminate target */
        s += strspn(s + 1, BASE_XML_WS) + 1;	/* skip whitespace after target */
    }

    if (!root)
        return;

    if (!strcmp(target, "xml")) {	/* <?xml ... ?> */
        if ((s = strstr(s, "standalone")) && !strncmp(s + strspn(s + 10, BASE_XML_WS "='\"") + 10, "yes", 3))
            root->standalone = 1;
        return;
    }

    if (root->pi == (char ***)(BASE_XML_NIL) || !root->pi || !root->pi[0]) {
        root->pi = (char ***) base_must_malloc(sizeof(char **));
        *(root->pi) = NULL;		/* first pi */
    }

    while (root->pi[i] && strcmp(target, root->pi[i][0]))
        i++;					/* find target */
    if (!root->pi[i]) {			/* new target */
        char ***ssstmp = (char ***) base_must_realloc(root->pi, sizeof(char **) * (i + 2));

        root->pi = ssstmp;
        if (!root->pi)
            return;
        root->pi[i] = (char **) base_must_malloc(sizeof(char *) * 3);
        root->pi[i][0] = target;
        root->pi[i][1] = (char *) (root->pi[i + 1] = NULL);	/* terminate pi list */
        root->pi[i][2] = base_must_strdup("");	/* empty document position list */
    }

    while (root->pi[i][j])
        j++;					/* find end of instruction list for this target */
    sstmp = (char **) base_must_realloc(root->pi[i], sizeof(char *) * (j + 3));
    root->pi[i] = sstmp;
    stmp = (char *) base_must_realloc(root->pi[i][j + 1], j + 1);
    root->pi[i][j + 2] = stmp;
    strcpy(root->pi[i][j + 2] + j - 1, (root->xml.name) ? ">" : "<");
    root->pi[i][j + 1] = NULL;	/* null terminate pi list for this target */
    root->pi[i][j] = s;			/* set instruction */
}

/* called when the parser finds an internal doctype subset */
static short base_xml_internal_dtd(base_xml_root_t root, char *s, base_size_t len)
{
    char q, *c, *t, *n = NULL, *v, **ent, **pe;
    int i, j;
    char **sstmp;

    pe = (char **) memcpy(base_must_malloc(sizeof(BASE_XML_NIL)), BASE_XML_NIL, sizeof(BASE_XML_NIL));

    for (s[len] = '\0'; s;) {
        while (*s && *s != '<' && *s != '%')
            s++;				/* find next declaration */

        if (!*s)
            break;
        else if (!strncmp(s, "<!ENTITY", 8)) {	/* parse entity definitions */
            int use_pe;

            c = s += strspn(s + 8, BASE_XML_WS) + 8;	/* skip white space separator */
            n = s + strspn(s, BASE_XML_WS "%");	/* find name */
            *(s = n + strcspn(n, BASE_XML_WS)) = ';';	/* append ; to name */

            v = s + strspn(s + 1, BASE_XML_WS) + 1;	/* find value */
            if ((q = *(v++)) != '"' && q != '\'') {	/* skip externals */
                s = strchr(s, '>');
                continue;
            }

            use_pe = (*c == '%');
            for (i = 0, ent = (use_pe) ? pe : root->ent; ent[i]; i++);
            sstmp = (char **) base_must_realloc(ent, (i + 3) * sizeof(char *));	/* space for next ent */
            ent = sstmp;
            if (use_pe)
                pe = ent;
            else
                root->ent = ent;

            *(++s) = '\0';		/* null terminate name */
            if ((s = strchr(v, q)))
                *(s++) = '\0';	/* null terminate value */
            ent[i + 1] = base_xml_decode(v, pe, '%');	/* set value */
            ent[i + 2] = NULL;	/* null terminate entity list */
            if (!base_xml_ent_ok(n, ent[i + 1], ent)) {	/* circular reference */
                if (ent[i + 1] != v)
                    free(ent[i + 1]);
                base_xml_err(root, v, "circular entity declaration &%s", n);
                break;
            } else
                ent[i] = n;		/* set entity name */
        } else if (!strncmp(s, "<!ATTLIST", 9)) {	/* parse default attributes */
            t = s + strspn(s + 9, BASE_XML_WS) + 9;	/* skip whitespace separator */
            if (!*t) {
                base_xml_err(root, t, "unclosed <!ATTLIST");
                break;
            }
            if (*(s = t + strcspn(t, BASE_XML_WS ">")) == '>')
                continue;
            else
                *s = '\0';		/* null terminate tag name */
            for (i = 0; root->attr[i] && n && strcmp(n, root->attr[i][0]); i++);

            //while (*(n = ++s + strspn(s, BASE_XML_WS)) && *n != '>') {
            // gcc 4.4 you are a creep
            for (;;) {
                s++;
                if (!(*(n = s + strspn(s, BASE_XML_WS)) && *n != '>')) {
                    break;
                }
                if (*(s = n + strcspn(n, BASE_XML_WS)))
                    *s = '\0';	/* attr name */
                else {
                    base_xml_err(root, t, "malformed <!ATTLIST");
                    break;
                }

                s += strspn(s + 1, BASE_XML_WS) + 1;	/* find next token */
                c = (strncmp(s, "CDATA", 5)) ? (char *) "*" : (char *) " ";	/* is it cdata? */
                if (!strncmp(s, "NOTATION", 8))
                    s += strspn(s + 8, BASE_XML_WS) + 8;
                s = (*s == '(') ? strchr(s, ')') : s + strcspn(s, BASE_XML_WS);
                if (!s) {
                    base_xml_err(root, t, "malformed <!ATTLIST");
                    break;
                }

                s += strspn(s, BASE_XML_WS ")");	/* skip white space separator */
                if (!strncmp(s, "#FIXED", 6))
                    s += strspn(s + 6, BASE_XML_WS) + 6;
                if (*s == '#') {	/* no default value */
                    s += strcspn(s, BASE_XML_WS ">") - 1;
                    if (*c == ' ')
                        continue;	/* cdata is default, nothing to do */
                    v = NULL;
                } else if ((*s == '"' || *s == '\'') &&	/* default value */
                    (s = strchr(v = s + 1, *s)))
                    *s = '\0';
                else {
                    base_xml_err(root, t, "malformed <!ATTLIST");
                    break;
                }

                if (!root->attr[i]) {	/* new tag name */
                    root->attr = (!i) ? (char ***) base_must_malloc(2 * sizeof(char **))
                        : (char ***) base_must_realloc(root->attr, (i + 2) * sizeof(char **));
                    root->attr[i] = (char **) base_must_malloc(2 * sizeof(char *));
                    root->attr[i][0] = t;	/* set tag name */
                    root->attr[i][1] = (char *) (root->attr[i + 1] = NULL);
                }

                for (j = 1; root->attr[i][j]; j += 3);	/* find end of list */
                sstmp = (char **) base_must_realloc(root->attr[i], (j + 4) * sizeof(char *));

                root->attr[i] = sstmp;
                root->attr[i][j + 3] = NULL;	/* null terminate list */
                root->attr[i][j + 2] = c;	/* is it cdata? */
                root->attr[i][j + 1] = (v) ? base_xml_decode(v, root->ent, *c) : NULL;
                root->attr[i][j] = n;	/* attribute name  */
            }
        } else if (!strncmp(s, "<!--", 4))
            s = strstr(s + 4, "-->");	/* comments */
        else if (!strncmp(s, "<?", 2)) {	/* processing instructions */
            if ((s = strstr(c = s + 2, "?>")))
                base_xml_proc_inst(root, c, s++ - c);
        } else if (*s == '<')
            s = strchr(s, '>');	/* skip other declarations */
        else if (*(s++) == '%' && !root->standalone)
            break;
    }

    free(pe);
    return !*root->err;
}

/* Converts a UTF-16 string to UTF-8. Returns a new string that must be freed
or NULL if no conversion was needed. */
static char *base_xml_str2utf8(char **s, base_size_t *len)
{
    char *u;
    base_size_t l = 0, sl, max = *len;
    long c, d;
    int b, be = (**s == '\xFE') ? 1 : (**s == '\xFF') ? 0 : -1;

    if (be == -1)
        return NULL;			/* not UTF-16 */

    if (*len <= 3)
        return NULL;

    u = (char *) base_must_malloc(max);
    for (sl = 2; sl < *len - 1; sl += 2) {
        c = (be) ? (((*s)[sl] & 0xFF) << 8) | ((*s)[sl + 1] & 0xFF)	/* UTF-16BE */
            : (((*s)[sl + 1] & 0xFF) << 8) | ((*s)[sl] & 0xFF);	/* UTF-16LE */
        if (c >= 0xD800 && c <= 0xDFFF && (sl += 2) < *len - 1) {	/* high-half */
            d = (be) ? (((*s)[sl] & 0xFF) << 8) | ((*s)[sl + 1] & 0xFF)
                : (((*s)[sl + 1] & 0xFF) << 8) | ((*s)[sl] & 0xFF);
            c = (((c & 0x3FF) << 10) | (d & 0x3FF)) + 0x10000;
        }

        while (l + 6 > max) {
            char *tmp;
            tmp = (char *) base_must_realloc(u, max += BASE_XML_BUFSIZE);
            u = tmp;
        }
        if (c < 0x80)
            u[l++] = (char) c;	/* US-ASCII subset */
        else {					/* multi-byte UTF-8 sequence */
            for (b = 0, d = c; d; d /= 2)
                b++;			/* bits in c */
            b = (b - 2) / 5;	/* bytes in payload */
            u[l++] = (char) ((0xFF << (7 - b)) | (c >> (6 * b)));	/* head */
            while (b)
                u[l++] = (char) (0x80 | ((c >> (6 * --b)) & 0x3F));	/* payload */
        }
    }
    return *s = (char *) base_must_realloc(u, *len = l);
}

/* frees a tag attribute list */
static void base_xml_free_attr(char **attr)
{
    int i, c = 0;
    char *m;

    if (!attr || attr == BASE_XML_NIL)
        return;					/* nothing to free */
    while (attr[c])
        c += 2;					/* find end of attribute list */
    m = attr[c + 1];			/* list of which names and values are malloced */
    for (i = c / 2 - 1; i >= 0 ; i--) {
        if (m[i] & BASE_XML_NAMEM)
            free(attr[i * 2]);
        if (m[i] & BASE_XML_TXTM)
            free(attr[(i * 2) + 1]);
    }
    free(m);
    free(attr);
}

BASE_DECLARE(base_xml_t) base_xml_parse_str_dynamic(char *s, base_bool_t dup)
{
    base_xml_root_t root;
    char *data;

    base_assert(s);
    data = dup ? base_must_strdup(s) : s;

    if ((root = (base_xml_root_t) base_xml_parse_str(data, strlen(data)))) {
        root->dynamic = 1;		/* Make sure we free the memory is base_xml_free() */
        return &root->xml;
    } else {
        if (dup) {
            free(data);
        }
        return NULL;
    }
}

/* parse the given xml string and return a base_xml structure */
BASE_DECLARE(base_xml_t) base_xml_parse_str(char *s, base_size_t len)
{
    base_xml_root_t root = (base_xml_root_t) base_xml_new(NULL);
    char q, e, *d, **attr, **a = NULL;	/* initialize a to avoid compile warning */
    int l, i, j;

    root->m = s;
    if (!len)
        return base_xml_err(root, s, "root tag missing");
    root->u = base_xml_str2utf8(&s, &len);	/* convert utf-16 to utf-8 */
    root->e = (root->s = s) + len;	/* record start and end of work area */

    e = s[len - 1];				/* save end char */
    s[len - 1] = '\0';			/* turn end char into null terminator */

    while (*s && *s != '<')
        s++;					/* find first tag */
    if (!*s)
        return base_xml_err(root, s, "root tag missing");

    for (;;) {
        attr = (char **) BASE_XML_NIL;
        d = ++s;

        if (isalpha((int) (*s)) || *s == '_' || *s == ':' || (int8_t) * s < '\0') {	/* new tag */
            if (!root->cur)
                return base_xml_err(root, d, "markup outside of root element");

            s += strcspn(s, BASE_XML_WS "/>");
            while (isspace((int) (*s)))
                *(s++) = '\0';	/* null terminate tag name */

            if (*s && *s != '/' && *s != '>')	/* find tag in default attr list */
                for (i = 0; (a = root->attr[i]) && strcmp(a[0], d); i++);

            for (l = 0; *s && *s != '/' && *s != '>'; l += 2) {	/* new attrib */
                attr = (l) ? (char **) base_must_realloc(attr, (l + 4) * sizeof(char *))
                    : (char **) base_must_malloc(4 * sizeof(char *));	/* allocate space */
                attr[l + 3] = (l) ? (char *) base_must_realloc(attr[l + 1], (l / 2) + 2)
                    : (char *) base_must_malloc(2);	/* mem for list of maloced vals */
                strcpy(attr[l + 3] + (l / 2), " ");	/* value is not malloced */
                attr[l + 2] = NULL;	/* null terminate list */
                attr[l + 1] = (char *) "";	/* temporary attribute value */
                attr[l] = s;	/* set attribute name */

                s += strcspn(s, BASE_XML_WS "=/>");
                if (*s == '=' || isspace((int) (*s))) {
                    *(s++) = '\0';	/* null terminate tag attribute name */
                    q = *(s += strspn(s, BASE_XML_WS "="));
                    if (q == '"' || q == '\'') {	/* attribute value */
                        attr[l + 1] = ++s;
                        while (*s && *s != q)
                            s++;
                        if (*s)
                            *(s++) = '\0';	/* null terminate attribute val */
                        else {
                            base_xml_free_attr(attr);
                            return base_xml_err(root, d, "missing %c", q);
                        }

                        for (j = 1; a && a[j] && strcmp(a[j], attr[l]); j += 3);
                        attr[l + 1] = base_xml_decode(attr[l + 1], root->ent, (a && a[j]) ? *a[j + 2] : ' ');
                        if (attr[l + 1] < d || attr[l + 1] > s)
                            attr[l + 3][l / 2] = BASE_XML_TXTM;	/* value malloced */
                    }
                }
                while (isspace((int) (*s)))
                    s++;
            }

            if (*s == '/') {	/* self closing tag */
                *(s++) = '\0';
                if ((*s && *s != '>') || (!*s && e != '>')) {
                    if (l)
                        base_xml_free_attr(attr);
                    return base_xml_err(root, d, "missing >");
                }
                base_xml_open_tag(root, d, s + 1, attr);
                base_xml_close_tag(root, d, s, NULL);
            } else if ((q = *s) == '>' || (!*s && e == '>')) {	/* open tag */
                *s = '\0';		/* temporarily null terminate tag name */
                base_xml_open_tag(root, d, s, attr);
                *s = q;
            } else {
                if (l)
                    base_xml_free_attr(attr);
                return base_xml_err(root, d, "missing >");
            }
        } else if (*s == '/') {	/* close tag */
            char *close_pos = d - 1;
            s += strcspn(d = s + 1, BASE_XML_WS ">") + 1;
            if (!(q = *s) && e != '>')
                return base_xml_err(root, d, "missing >");
            *s = '\0';			/* temporarily null terminate tag name */
            if (base_xml_close_tag(root, d, s, close_pos))
                return &root->xml;
            if (isspace((int) (*s = q)))
                s += strspn(s, BASE_XML_WS);
        } else if (!strncmp(s, "!--", 3)) {	/* xml comment */
            if (!(s = strstr(s + 3, "--")) || (*(s += 2) != '>' && *s) || (!*s && e != '>'))
                return base_xml_err(root, d, "unclosed <!--");
        } else if (!strncmp(s, "![CDATA[", 8)) {	/* cdata */
            if ((s = strstr(s, "]]>"))) {
                if (root->cur) {
                    root->cur->flags |= BASE_XML_CDATA;
                }
                base_xml_char_content(root, d + 8, (s += 2) - d - 10, 'c');
            } else {
                return base_xml_err(root, d, "unclosed <![CDATA[");
            }
        } else if (!strncmp(s, "!DOCTYPE", 8)) {	/* dtd */
            for (l = 0; *s && ((!l && *s != '>') || (l && (*s != ']' || *(s + strspn(s + 1, BASE_XML_WS) + 1) != '>'))); l = (*s == '[') ? 1 : l)
                s += strcspn(s + 1, "[]>") + 1;
            if (!*s && e != '>')
                return base_xml_err(root, d, "unclosed <!DOCTYPE");
            d = (l) ? strchr(d, '[') + 1 : d;
            if (l && !base_xml_internal_dtd(root, d, s++ - d))
                return &root->xml;
        } else if (*s == '?') {	/* <?...?> processing instructions */
            do {
                s = strchr(s, '?');
            } while (s && *(++s) && *s != '>');
            if (!s || (!*s && e != '>'))
                return base_xml_err(root, d, "unclosed <?");
            else
                base_xml_proc_inst(root, d + 1, s - d - 2);
        } else
            return base_xml_err(root, d, "unexpected <");

        if (!s || !*s)
            break;
        *s = '\0';
        d = ++s;
        if (*s && *s != '<') {	/* tag character content */
            while (*s && *s != '<')
                s++;
            if (*s)
                base_xml_char_content(root, d, s - d, '&');
            else
                break;
        } else if (!*s)
            break;
    }

    if (!root->cur)
        return &root->xml;
    else if (!root->cur->name)
        return base_xml_err(root, d, "root tag missing");
    else
        return base_xml_err(root, d, "unclosed tag <%s>", root->cur->name);
}

/* Wrapper for base_xml_parse_str() that accepts a file stream. Reads the entire
stream into memory and then parses it. For xml files, use base_xml_parse_file()
or base_xml_parse_fd() */
BASE_DECLARE(base_xml_t) base_xml_parse_fp(FILE * fp)
{
    base_xml_root_t root;
    base_size_t l, len = 0;
    char *s;

    s = (char *) base_must_malloc(BASE_XML_BUFSIZE);

    do {
        len += (l = fread((s + len), 1, BASE_XML_BUFSIZE, fp));
        if (l == BASE_XML_BUFSIZE) {
            s = (char *) base_must_realloc(s, len + BASE_XML_BUFSIZE);
        }
    } while (s && l == BASE_XML_BUFSIZE);

    if (!s) {
        return NULL;
    }

    if (!(root = (base_xml_root_t) base_xml_parse_str(s, len))) {
        free(s);

        return NULL;
    }

    root->dynamic = 1;			/* so we know to free s in base_xml_free() */

    return &root->xml;
}

/* A wrapper for base_xml_parse_str() that accepts a file descriptor. First
attempts to mem map the file. Failing that, reads the file into memory.
Returns NULL on failure. */
BASE_DECLARE(base_xml_t) base_xml_parse_fd(int fd)
{
    base_xml_root_t root;
    struct stat st;
    base_ssize_t l;
    void *m;

    if (fd < 0)
        return NULL;

    if (fstat(fd, &st) == -1 || !st.st_size) {
        return NULL;
    }

    m = base_must_malloc(st.st_size);

    if (!(0<(l = read(fd, m, st.st_size)))
        || !(root = (base_xml_root_t) base_xml_parse_str((char *) m, l))) {
        free(m);

        return NULL;
    }

    root->dynamic = 1;		/* so we know to free s in base_xml_free() */

    return &root->xml;
}

static char *expand_vars(char *buf, char *ebuf, base_size_t elen, base_size_t *newlen, const char **err)
{
    char *var, *val;
    char *rp = buf;
    char *wp = ebuf;
    char *ep = ebuf + elen - 1;

    if (!strstr(rp, "$${")) {
        *newlen = strlen(buf);
        return buf;
    }

    while (*rp && wp < ep) {

        if (*rp == '$' && *(rp + 1) == '$' && *(rp + 2) == '{') {
            char *e = base_find_end_paren(rp + 2, '{', '}');

            if (e) {
                rp += 3;
                var = rp;
                *e++ = '\0';
                rp = e;
                if ((val = base_core_get_variable_dup(var))) {
                    char *p;
                    for (p = val; p && *p && wp <= ep; p++) {
                        *wp++ = *p;
                    }
                    free(val);
                }
                continue;
            } else if (err) {
                *err = "unterminated ${var}";
            }
        }

        *wp++ = *rp++;
    }

    if (wp == ep) {
        return NULL;
    }

    *wp++ = '\0';
    *newlen = strlen(ebuf);

    return ebuf;
}

static FILE *preprocess_exec(const char *cwd, const char *command, FILE *write_fd, int rlevel)
{
#ifdef WIN32
    FILE *fp = NULL;
    char buffer[1024];

    if (!command || !strlen(command)) goto end;

    if ((fp = _popen(command, "r"))) {
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (fwrite(buffer, 1, strlen(buffer), write_fd) <= 0) {
                break;
            }
        }

        if(feof(fp)) {
            _pclose(fp);
        } else {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Exec failed to read the pipe of [%s] to the end\n", command);
        }
    } else {
        base_snprintf(buffer, sizeof(buffer), "<!-- exec can not execute [%s] -->", command);
        fwrite( buffer, 1, strlen(buffer), write_fd);
    }
#else
    int fds[2], pid = 0;

    if (pipe(fds)) {
        goto end;
    } else {					/* good to go */
        pid = base_fork();

        if (pid < 0) {			/* ok maybe not */
            close(fds[0]);
            close(fds[1]);
            goto end;
        } else if (pid) {		/* parent */
            char buf[1024] = "";
            int bytes;
            close(fds[1]);
            while ((bytes = read(fds[0], buf, sizeof(buf))) > 0) {
                if (fwrite(buf, 1, bytes, write_fd) <= 0) {
                    break;
                }
            }
            close(fds[0]);
            waitpid(pid, NULL, 0);
        } else {				/*  child */
            base_close_extra_files(fds, 2);
            close(fds[0]);
            dup2(fds[1], STDOUT_FILENO);
            base_system(command, BASE_TRUE);
            close(fds[1]);
            exit(0);
        }
    }
#endif
end:

    return write_fd;

}

static FILE *preprocess_glob(const char *cwd, const char *pattern, FILE *write_fd, int rlevel)
{
    char *full_path = NULL;
    char *dir_path = NULL, *e = NULL;
    glob_t glob_data;
    size_t n;
    int glob_return;

    if (!base_is_file_path(pattern)) {
        full_path = base_mprintf("%s%s%s", cwd, BASE_PATH_SEPARATOR, pattern);
        pattern = full_path;
    }

    glob_return = glob(pattern, GLOB_ERR, NULL, &glob_data);
    if (glob_return == GLOB_NOSPACE || glob_return == GLOB_ABORTED) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Error including %s\n", pattern);
        goto end;
    } else if (glob_return == GLOB_NOMATCH) {
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_INFO, "No files to include at %s\n", pattern);
        goto end;
    }

    for (n = 0; n < glob_data.gl_pathc; ++n) {
        dir_path = base_must_strdup(glob_data.gl_pathv[n]);

        if ((e = strrchr(dir_path, *BASE_PATH_SEPARATOR))) {
            *e = '\0';
        }
        if (preprocess(dir_path, glob_data.gl_pathv[n], write_fd, rlevel) < 0) {
            if (rlevel > 100) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Error including %s (Maximum recursion limit reached)\n", pattern);
            }
        }
        free(dir_path);
    }
    globfree(&glob_data);

end:

    base_safe_free(full_path);

    return write_fd;
}

static int preprocess(const char *cwd, const char *file, FILE *write_fd, int rlevel)
{
    FILE *read_fd = NULL;
    base_size_t cur = 0, ml = 0;
    char *q, *cmd, *buf = NULL, *ebuf = NULL;
    char *tcmd, *targ;
    int line = 0;
    base_size_t len = 0, eblen = 0;

    if (rlevel > 100) {
        return -1;
    }

    if (!(read_fd = fopen(file, "r"))) {
        const char *reason = strerror(errno);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Couldn't open %s (%s)\n", file, reason);
        return -1;
    }

    setvbuf(read_fd, (char *) NULL, _IOFBF, 65536);

    for(;;) {
        char *arg, *e;
        const char *err = NULL;
        char *bp;

        base_safe_free(ebuf);

        if ((cur = base_fp_read_dline(read_fd, &buf, &len)) <= 0) {
            break;
        }

        eblen = len * 2;
        ebuf = base_must_malloc(eblen);
        memset(ebuf, 0, eblen);

        while (!(bp = expand_vars(buf, ebuf, eblen, &cur, &err))) {
            eblen *= 2;
            ebuf = base_must_realloc(ebuf, eblen);
            memset(ebuf, 0, eblen);
        }

        line++;

        if (err) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Error [%s] in file %s line %d\n", err, file, line);
        }

        /* we ignore <include> or </include> for the sake of validators as well as <?xml version="1.0"?> type stuff */
        if (strstr(buf, "<include>") || strstr(buf, "</include>") || strstr(buf, "<?")) {
            continue;
        }

        if (ml) {
            if ((e = strstr(buf, "-->"))) {
                ml = 0;
                bp = e + 3;
                cur = strlen(bp);
            } else {
                continue;
            }
        }

        if ((tcmd = (char *) base_stristr("X-pre-process", bp))) {
            if (*(tcmd - 1) != '<') {
                continue;
            }
            if ((e = strstr(tcmd, "/>"))) {
                e += 2;
                *e = '\0';
                if (fwrite(e, 1, (unsigned) strlen(e), write_fd) != (int) strlen(e)) {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Short write!\n");
                }
            }

            if (!(tcmd = (char *) base_stristr("cmd", tcmd))) {
                continue;
            }

            if (!(tcmd = (char *) base_stristr("=", tcmd))) {
                continue;
            }

            if (!(tcmd = (char *) base_stristr("\"", tcmd))) {
                continue;
            }

            tcmd++;


            if ((e = strchr(tcmd, '"'))) {
                *e++ = '\0';
            }

            if (!(targ = (char *) base_stristr("data", e))) {
                continue;
            }

            if (!(targ = (char *) base_stristr("=", targ))) {
                continue;
            }

            if (!(targ = (char *) base_stristr("\"", targ))) {
                continue;
            }

            targ++;

            if ((e = strchr(targ, '"'))) {
                *e++ = '\0';
            }

            if (!strcasecmp(tcmd, "set")) {
                char *name = (char *) targ;
                char *val = strchr(name, '=');

                if (val) {
                    char *ve = val++;
                    while (*val && *val == ' ') {
                        val++;
                    }
                    *ve-- = '\0';
                    while (*ve && *ve == ' ') {
                        *ve-- = '\0';
                    }
                }

                if (val) {
                    base_core_set_variable(name, val);
                }

            } else if (!strcasecmp(tcmd, "exec-set")) {
                preprocess_exec_set(targ);
            } else if (!strcasecmp(tcmd, "stun-set")) {
                preprocess_stun_set(targ);
            } else if (!strcasecmp(tcmd, "env-set")) {
                preprocess_env_set(targ);
            } else if (!strcasecmp(tcmd, "include")) {
                preprocess_glob(cwd, targ, write_fd, rlevel + 1);
            } else if (!strcasecmp(tcmd, "exec")) {
                preprocess_exec(cwd, targ, write_fd, rlevel + 1);
            }

            continue;
        }

        if ((cmd = strstr(bp, "<!--#"))) {
            if (fwrite(bp, 1, (unsigned) (cmd - bp), write_fd) != (unsigned) (cmd - bp)) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Short write!\n");
            }
            if ((e = strstr(cmd, "-->"))) {
                *e = '\0';
                e += 3;
                if (fwrite(e, 1, (unsigned) strlen(e), write_fd) != (int) strlen(e)) {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Short write!\n");
                }
            } else {
                ml++;
            }

            cmd += 5;
            if ((e = strchr(cmd, '\r')) || (e = strchr(cmd, '\n'))) {
                *e = '\0';
            }

            if ((arg = strchr(cmd, ' '))) {
                *arg++ = '\0';
                if ((q = strchr(arg, '"'))) {
                    char *qq = q + 1;

                    if ((qq = strchr(qq, '"'))) {
                        *qq = '\0';
                        arg = q + 1;
                    }
                }

                if (!strcasecmp(cmd, "set")) {
                    char *name = arg;
                    char *val = strchr(name, '=');

                    if (val) {
                        char *ve = val++;
                        while (*val && *val == ' ') {
                            val++;
                        }
                        *ve-- = '\0';
                        while (*ve && *ve == ' ') {
                            *ve-- = '\0';
                        }
                    }

                    if (val) {
                        base_core_set_variable(name, val);
                    }

                } else if (!strcasecmp(cmd, "exec-set")) {
                    preprocess_exec_set(arg);
                } else if (!strcasecmp(cmd, "stun-set")) {
                    preprocess_stun_set(arg);
                } else if (!strcasecmp(cmd, "include")) {
                    preprocess_glob(cwd, arg, write_fd, rlevel + 1);
                } else if (!strcasecmp(cmd, "exec")) {
                    preprocess_exec(cwd, arg, write_fd, rlevel + 1);
                }
            }

            continue;
        }

        if (fwrite(bp, 1, (unsigned) cur, write_fd) != (int) cur) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Short write!\n");
        }

    }

    base_safe_free(buf);
    base_safe_free(ebuf);

    fclose(read_fd);

    return 0;
}

BASE_DECLARE(base_xml_t) base_xml_parse_file_simple(const char *file)
{
    int fd = -1;
    struct stat st;
    base_ssize_t l;
    void *m;
    base_xml_root_t root;

    if ((fd = open(file, O_RDONLY, 0)) > -1) {
        if (fstat(fd, &st) == -1 || !st.st_size) {
            close(fd);
            goto error;
        }

        m = base_must_malloc(st.st_size);

        if (!(0 < (l = read(fd, m, st.st_size)))) {
            free(m);
            close(fd);
            goto error;
        }

        if (!(root = (base_xml_root_t)base_xml_parse_str((char*)m, l))) {
            free(m);
            close(fd);
            goto error;
        }

        root->dynamic = 1;
        close(fd);

        return &root->xml;
    }

error:

    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Error Parsing File [%s]\n", file);

    return NULL;
}

BASE_DECLARE(base_xml_t) base_xml_parse_file(const char *file)
{
    int fd = -1;
    FILE *write_fd = NULL;
    base_xml_t xml = NULL;
    char *new_file = NULL;
    char *new_file_tmp = NULL;
    const char *abs, *absw;

    abs = strrchr(file, '/');
    absw = strrchr(file, '\\');
    if (abs || absw) {
        abs > absw ? abs++ : (abs = ++absw);
    } else {
        abs = file;
    }

    base_mutex_lock(FILE_LOCK);

    if (!(new_file = base_mprintf("%s%s%s.fsxml", BASE_GLOBAL_dirs.log_dir, BASE_PATH_SEPARATOR, abs))) {
        goto done;
    }

    if (!(new_file_tmp = base_mprintf("%s%s%s.fsxml.tmp", BASE_GLOBAL_dirs.log_dir, BASE_PATH_SEPARATOR, abs))) {
        goto done;
    }

    if ((write_fd = fopen(new_file_tmp, "w+")) == NULL) {
        goto done;
    }

    setvbuf(write_fd, (char *) NULL, _IOFBF, 65536);

    if (preprocess(BASE_GLOBAL_dirs.conf_dir, file, write_fd, 0) > -1) {
        fclose(write_fd);
        write_fd = NULL;
        unlink (new_file);

        if ( rename(new_file_tmp,new_file) ) {
            goto done;
        }

        if ((fd = open(new_file, O_RDONLY, 0)) > -1) {
            if ((xml = base_xml_parse_fd(fd))) {
                if (strcmp(abs, BASE_GLOBAL_filenames.conf_name)) {
                    xml->free_path = new_file;
                    new_file = NULL;
                }
            }

            close(fd);
        }
    }

done:

    base_mutex_unlock(FILE_LOCK);

    if (write_fd) {
        fclose(write_fd);
        write_fd = NULL;
    }

    base_safe_free(new_file_tmp);
    base_safe_free(new_file);

    return xml;
}

BASE_DECLARE(base_status_t) base_xml_locate(const char *section,
    const char *tag_name,
    const char *key_name,
    const char *key_value,
    base_xml_t *root, base_xml_t *node, base_event_t *params, base_bool_t clone)
{
    base_xml_t conf = NULL;
    base_xml_t tag = NULL;
    base_xml_t xml = NULL;
    base_xml_binding_t *binding;
    uint8_t loops = 0;
    base_xml_section_t sections = BINDINGS ? base_xml_parse_section_string(section) : 0;

    base_thread_rwlock_rdlock(B_RWLOCK);

    for (binding = BINDINGS; binding; binding = binding->next) {
        if (binding->sections && !(sections & binding->sections)) {
            continue;
        }

        if ((xml = binding->function(section, tag_name, key_name, key_value, params, binding->user_data))) {
            const char *err = NULL;

            err = base_xml_error(xml);
            if (zstr(err)) {
                if ((conf = base_xml_find_child(xml, "section", "name", "result"))) {
                    base_xml_t p;
                    const char *aname;

                    if ((p = base_xml_child(conf, "result"))) {
                        aname = base_xml_attr(p, "status");
                        if (aname && !strcasecmp(aname, "not found")) {
                            base_xml_free(xml);
                            xml = NULL;
                            continue;
                        }
                    }
                }
                break;
            } else {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Error[%s]\n", err);
                base_xml_free(xml);
                xml = NULL;
            }
        }
    }
    base_thread_rwlock_unlock(B_RWLOCK);

    for (;;) {
        if (!xml) {
            if (!(xml = base_xml_root())) {
                *node = NULL;
                *root = NULL;
                return BASE_STATUS_FALSE;
            }
        }

        if ((conf = base_xml_find_child(xml, "section", "name", section)) && (tag = base_xml_find_child(conf, tag_name, key_name, key_value))) {
            if (clone) {
                char *x = base_xml_toxml(tag, BASE_FALSE);
                base_assert(x);
                *node = *root = base_xml_parse_str_dynamic(x, BASE_FALSE);	/* x will be free()'d in base_xml_free() */
                base_xml_free(xml);
            } else {
                *node = tag;
                *root = xml;
            }
            return BASE_STATUS_SUCCESS;
        } else {
            base_xml_free(xml);
            xml = NULL;
            *node = NULL;
            *root = NULL;
            if (loops++ > 1) {
                break;
            }
        }
    }

    return BASE_STATUS_FALSE;
}

BASE_DECLARE(base_status_t) base_xml_locate_domain(const char *domain_name, base_event_t *params, base_xml_t *root, base_xml_t *domain)
{
    base_event_t *my_params = NULL;
    base_status_t status;
    *domain = NULL;

    if (!params) {
        base_event_create(&my_params, BASE_EVENT_REQUEST_PARAMS);
        base_assert(my_params);
        base_event_add_header_string(my_params, BASE_STACK_BOTTOM, "domain", domain_name);
        params = my_params;
    }

    status = base_xml_locate("directory", "domain", "name", domain_name, root, domain, params, BASE_FALSE);
    if (my_params) {
        base_event_destroy(&my_params);
    }
    return status;
}

BASE_DECLARE(base_status_t) base_xml_locate_group(const char *group_name,
    const char *domain_name,
    base_xml_t *root, base_xml_t *domain, base_xml_t *group, base_event_t *params)
{
    base_status_t status = BASE_STATUS_FALSE;
    base_event_t *my_params = NULL;
    base_xml_t groups = NULL;

    *root = NULL;
    *group = NULL;
    *domain = NULL;

    if (!params) {
        base_event_create(&my_params, BASE_EVENT_REQUEST_PARAMS);
        base_assert(my_params);
        params = my_params;
    }

    if (group_name) {
        base_event_add_header_string(params, BASE_STACK_BOTTOM, "group_name", group_name);
    }

    if (domain_name) {
        base_event_add_header_string(params, BASE_STACK_BOTTOM, "domain", domain_name);
    }

    if ((status = base_xml_locate_domain(domain_name, params, root, domain)) != BASE_STATUS_SUCCESS) {
        goto end;
    }

    status = BASE_STATUS_FALSE;

    if ((groups = base_xml_child(*domain, "groups"))) {
        if ((*group = base_xml_find_child(groups, "group", "name", group_name))) {
            status = BASE_STATUS_SUCCESS;
        }
    }

end:

    if (my_params) {
        base_event_destroy(&my_params);
    }

    return status;
}

static base_status_t find_user_in_tag(base_xml_t tag, const char *ip, const char *user_name,
    const char *key, base_event_t *params, base_xml_t *user)
{
    const char *type = "!pointer";
    const char *val;

    if (params && (val = base_event_get_header(params, "user_type"))) {
        if (!strcasecmp(val, "any")) {
            type = NULL;
        } else {
            type = val;
        }
    }

    if (ip) {
        if ((*user = base_xml_find_child_multi(tag, "user", "ip", ip, "type", type, NULL))) {
            return BASE_STATUS_SUCCESS;
        }
    }

    if (user_name) {
        if (!strcasecmp(key, "id")) {
            if ((*user = base_xml_find_child_multi(tag, "user", key, user_name, "number-alias", user_name, "type", type, NULL))) {
                return BASE_STATUS_SUCCESS;
            }
        } else {
            if ((*user = base_xml_find_child_multi(tag, "user", key, user_name, "type", type, NULL))) {
                return BASE_STATUS_SUCCESS;
            }
        }
    }

    return BASE_STATUS_FALSE;

}

BASE_DECLARE(base_status_t) base_xml_locate_user_in_domain(const char *user_name, base_xml_t domain, base_xml_t *user, base_xml_t *ingroup)
{
    base_xml_t group = NULL, groups = NULL, users = NULL;
    base_status_t status = BASE_STATUS_FALSE;

    if ((groups = base_xml_child(domain, "groups"))) {
        for (group = base_xml_child(groups, "group"); group; group = group->next) {
            if ((users = base_xml_child(group, "users"))) {
                if ((status = find_user_in_tag(users, NULL, user_name, "id", NULL, user)) == BASE_STATUS_SUCCESS) {
                    if (ingroup) {
                        *ingroup = group;
                    }
                    break;
                }
            }
        }
    } else {
        if ((users = base_xml_child(domain, "users"))) {
            status = find_user_in_tag(users, NULL, user_name, "id", NULL, user);
        } else {
            status = find_user_in_tag(domain, NULL, user_name, "id", NULL, user);
        }
    }

    return status;
}


BASE_DECLARE(base_xml_t) base_xml_dup(base_xml_t xml)
{
    char *x = base_xml_toxml(xml, BASE_FALSE);
    return base_xml_parse_str_dynamic(x, BASE_FALSE);
}


static void do_merge(base_xml_t in, base_xml_t src, const char *container, const char *tag_name)
{
    base_xml_t itag, tag, param, iparam, iitag;

    if (!(itag = base_xml_child(in, container))) {
        itag = base_xml_add_child_d(in, container, 0);
    }

    if ((tag = base_xml_child(src, container))) {
        for (param = base_xml_child(tag, tag_name); param; param = param->next) {
            const char *var = base_xml_attr(param, "name");
            const char *val = base_xml_attr(param, "value");

            base_bool_t add_child = BASE_TRUE;

            for (iparam = base_xml_child(itag, tag_name); iparam; iparam = iparam->next) {
                const char *ivar = base_xml_attr(iparam, "name");

                if (var && ivar && !strcasecmp(var, ivar)) {
                    add_child = BASE_FALSE;
                    break;
                }
            }

            if (add_child) {
                iitag = base_xml_add_child_d(itag, tag_name, 0);
                base_xml_set_attr_d(iitag, "name", var);
                base_xml_set_attr_d(iitag, "value", val);
            }
        }
    }

}


BASE_DECLARE(void) base_xml_merge_user(base_xml_t user, base_xml_t domain, base_xml_t group)
{
    const char *domain_name = base_xml_attr(domain, "name");

    do_merge(user, group, "params", "param");
    do_merge(user, group, "variables", "variable");
    do_merge(user, group, "profile-variables", "variable");
    do_merge(user, domain, "params", "param");
    do_merge(user, domain, "variables", "variable");
    do_merge(user, domain, "profile-variables", "variable");

    if (!zstr(domain_name)) {
        base_xml_set_attr_d(user, "domain-name", domain_name);
    }
}

BASE_DECLARE(uint32_t) base_xml_clear_user_cache(const char *key, const char *user_name, const char *domain_name)
{
    base_hash_index_t *hi = NULL;
    void *val;
    const void *var;
    char mega_key[1024];
    int r = 0;
    base_xml_t lookup;
    char *expires_val = NULL;

    base_mutex_lock(CACHE_MUTEX);

    if (key && user_name && !domain_name) {
        domain_name = base_core_get_variable("domain");
    }

    if (key && user_name && domain_name) {
        base_snprintf(mega_key, sizeof(mega_key), "%s%s%s", key, user_name, domain_name);

        if ((lookup = base_core_hash_find(CACHE_HASH, mega_key))) {
            base_core_hash_delete(CACHE_HASH, mega_key);
            if ((expires_val = base_core_hash_find(CACHE_EXPIRES_HASH, mega_key))) {
                base_core_hash_delete(CACHE_EXPIRES_HASH, mega_key);
                free(expires_val);
                expires_val = NULL;
            }
            base_xml_free(lookup);
            r++;
        }

    } else {

        while ((hi = base_core_hash_first_iter( CACHE_HASH, hi))) {
            base_core_hash_this(hi, &var, NULL, &val);
            base_xml_free(val);
            base_core_hash_delete(CACHE_HASH, var);
            r++;
        }

        while ((hi = base_core_hash_first_iter( CACHE_EXPIRES_HASH, hi))) {
            base_core_hash_this(hi, &var, NULL, &val);
            base_safe_free(val);
            base_core_hash_delete(CACHE_EXPIRES_HASH, var);
        }

        base_safe_free(hi);
    }

    base_mutex_unlock(CACHE_MUTEX);

    return r;

}

static base_status_t base_xml_locate_user_cache(const char *key, const char *user_name, const char *domain_name, base_xml_t *user)
{
    char mega_key[1024];
    base_status_t status = BASE_STATUS_FALSE;
    base_xml_t lookup;

    base_snprintf(mega_key, sizeof(mega_key), "%s%s%s", key, user_name, domain_name);

    base_mutex_lock(CACHE_MUTEX);
    if ((lookup = base_core_hash_find(CACHE_HASH, mega_key))) {
        char *expires_lookup = NULL;

        if ((expires_lookup = base_core_hash_find(CACHE_EXPIRES_HASH, mega_key))) {
            base_time_t time_expires = 0;
            base_time_t time_now = 0;

            time_now = base_micro_time_now();
            time_expires = atol(expires_lookup);
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "Cache Info\nTime Now:\t%ld\nExpires:\t%ld\n", (long)time_now, (long)time_expires);
            if (time_expires < time_now) {
                base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "Cache expired for %s@%s, doing fresh lookup\n", user_name, domain_name);
            } else {
                *user = base_xml_dup(lookup);
                status = BASE_STATUS_SUCCESS;
            }
        } else {
            *user = base_xml_dup(lookup);
            status = BASE_STATUS_SUCCESS;
        }
    }
    base_mutex_unlock(CACHE_MUTEX);

    return status;
}

static void base_xml_user_cache(const char *key, const char *user_name, const char *domain_name, base_xml_t user, base_time_t expires)
{
    char mega_key[1024];
    base_xml_t lookup;
    char *expires_lookup;

    base_snprintf(mega_key, sizeof(mega_key), "%s%s%s", key, user_name, domain_name);

    base_mutex_lock(CACHE_MUTEX);
    if ((lookup = base_core_hash_find(CACHE_HASH, mega_key))) {
        base_core_hash_delete(CACHE_HASH, mega_key);
        base_xml_free(lookup);
    }
    if ((expires_lookup = base_core_hash_find(CACHE_EXPIRES_HASH, mega_key))) {
        base_core_hash_delete(CACHE_EXPIRES_HASH, mega_key);
        base_safe_free(expires_lookup);
    }
    if (expires) {
        char *expires_val = (char *)base_core_hash_insert_alloc(CACHE_EXPIRES_HASH, mega_key, 22);
        if (!snprintf(expires_val, 22, "%ld", (long)expires)) {
            base_core_hash_delete(CACHE_EXPIRES_HASH, mega_key);
            base_safe_free(expires_val);
        }
    }
    base_core_hash_insert(CACHE_HASH, mega_key, base_xml_dup(user));
    base_mutex_unlock(CACHE_MUTEX);
}

BASE_DECLARE(base_status_t) base_xml_locate_user_merged(const char *key, const char *user_name, const char *domain_name,
    const char *ip, base_xml_t *user, base_event_t *params)
{
    base_xml_t xml, domain, group, x_user, x_user_dup;
    base_status_t status = BASE_STATUS_FALSE;
    char *kdup = NULL;
    char *keys[10] = {0};
    int i, nkeys;

    if (strchr(key, ':')) {
        kdup = base_must_strdup(key);
        nkeys  = base_split(kdup, ':', keys);
    } else {
        keys[0] = (char *)key;
        nkeys = 1;
    }

    for(i = 0; i < nkeys; i++) {
        if ((status = base_xml_locate_user_cache(keys[i], user_name, domain_name, &x_user)) == BASE_STATUS_SUCCESS) {
            *user = x_user;
            break;
        } else if ((status = base_xml_locate_user(keys[i], user_name, domain_name, ip, &xml, &domain, &x_user, &group, params)) == BASE_STATUS_SUCCESS) {
            const char *cacheable = NULL;

            x_user_dup = base_xml_dup(x_user);
            base_xml_merge_user(x_user_dup, domain, group);

            cacheable = base_xml_attr(x_user_dup, "cacheable");
            if (!zstr(cacheable)) {
                base_time_t expires = 0;
                base_time_t time_now = 0;

                if (base_is_number(cacheable)) {
                    int cache_ms = atol(cacheable);
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "caching lookup for user %s@%s for %d milliseconds\n",
                        user_name, domain_name, cache_ms);
                    time_now = base_micro_time_now();
                    expires = time_now + (cache_ms * 1000);
                } else {
                    base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG, "caching lookup for user %s@%s indefinitely\n", user_name, domain_name);
                }
                base_xml_user_cache(keys[i], user_name, domain_name, x_user_dup, expires);
            }
            *user = x_user_dup;
            base_xml_free(xml);
            break;
        }
    }

    base_safe_free(kdup);

    return status;

}

BASE_DECLARE(base_status_t) base_xml_locate_user(const char *key,
    const char *user_name,
    const char *domain_name,
    const char *ip,
    base_xml_t *root,
    base_xml_t *domain, base_xml_t *user, base_xml_t *ingroup, base_event_t *params)
{
    base_status_t status = BASE_STATUS_FALSE;
    base_event_t *my_params = NULL;
    base_xml_t group = NULL, groups = NULL, users = NULL;

    *root = NULL;
    *user = NULL;
    *domain = NULL;

    if (ingroup) {
        *ingroup = NULL;
    }

    if (!params) {
        base_event_create(&my_params, BASE_EVENT_REQUEST_PARAMS);
        base_assert(my_params);
        params = my_params;
    }

    base_event_add_header_string(params, BASE_STACK_BOTTOM, "key", key);

    if (user_name) {
        base_event_add_header_string(params, BASE_STACK_BOTTOM, "user", user_name);
    }

    if (domain_name) {
        base_event_add_header_string(params, BASE_STACK_BOTTOM, "domain", domain_name);
    }

    if (ip) {
        base_event_add_header_string(params, BASE_STACK_BOTTOM, "ip", ip);
    }

    if ((status = base_xml_locate_domain(domain_name, params, root, domain)) != BASE_STATUS_SUCCESS) {
        goto end;
    }

    status = BASE_STATUS_FALSE;

    if ((groups = base_xml_child(*domain, "groups"))) {
        for (group = base_xml_child(groups, "group"); group; group = group->next) {
            if ((users = base_xml_child(group, "users"))) {
                if ((status = find_user_in_tag(users, ip, user_name, key, params, user)) == BASE_STATUS_SUCCESS) {
                    if (ingroup) {
                        *ingroup = group;
                    }
                    break;
                }
            }
        }
    }

    if (status != BASE_STATUS_SUCCESS) {
        if ((users = base_xml_child(*domain, "users"))) {
            status = find_user_in_tag(users, ip, user_name, key, params, user);
        } else {
            status = find_user_in_tag(*domain, ip, user_name, key, params, user);
        }
    }

end:

    if (my_params) {
        base_event_destroy(&my_params);
    }

    if (status != BASE_STATUS_SUCCESS && *root) {
        base_xml_free(*root);
        *root = NULL;
        *domain = NULL;
    }

    return status;
}

BASE_DECLARE(base_xml_t) base_xml_root(void)
{
    base_xml_t xml;

    base_mutex_lock(REFLOCK);
    xml = MAIN_XML_ROOT;
    xml->refs++;
    base_mutex_unlock(REFLOCK);

    return xml;
}

struct destroy_xml {
    base_xml_t xml;
    base_memory_pool_t *pool;
};

static void *BASE_THREAD_FUNC destroy_thread(base_thread_t *thread, void *obj)
{
    struct destroy_xml *dx = (struct destroy_xml *) obj;
    base_memory_pool_t *pool = dx->pool;
    base_xml_free(dx->xml);
    base_core_destroy_memory_pool(&pool);
    return NULL;
}

BASE_DECLARE(void) base_xml_free_in_thread(base_xml_t xml, int stacksize)
{
    base_thread_t *thread;
    base_threadattr_t *thd_attr;
    base_memory_pool_t *pool = NULL;
    struct destroy_xml *dx;

    base_core_new_memory_pool(&pool);

    base_threadattr_create(&thd_attr, pool);
    base_threadattr_detach_set(thd_attr, 1);
    /* TBD figure out how much space we need by looking at the xml_t when stacksize == 0 */
    base_threadattr_stacksize_set(thd_attr, stacksize);

    dx = base_core_alloc(pool, sizeof(*dx));
    dx->pool = pool;
    dx->xml = xml;

    base_thread_create(&thread, thd_attr, destroy_thread, dx, pool);
}

static char not_so_threadsafe_error_buffer[256] = "";

BASE_DECLARE(base_status_t) base_xml_set_root(base_xml_t new_main)
{
    base_xml_t old_root = NULL;

    base_mutex_lock(REFLOCK);

    old_root = MAIN_XML_ROOT;
    MAIN_XML_ROOT = new_main;
    base_set_flag(MAIN_XML_ROOT, BASE_XML_ROOT);
    MAIN_XML_ROOT->refs++;

    if (old_root) {
        if (old_root->refs) {
            old_root->refs--;
        }

        if (!old_root->refs) {
            base_xml_free(old_root);
        }
    }

    base_mutex_unlock(REFLOCK);

    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(base_status_t) base_xml_set_open_root_function(base_xml_open_root_function_t func, void *user_data)
{
    if (XML_LOCK) {
        base_mutex_lock(XML_LOCK);
    }

    XML_OPEN_ROOT_FUNCTION = func;
    XML_OPEN_ROOT_FUNCTION_USER_DATA = user_data;

    if (XML_LOCK) {
        base_mutex_unlock(XML_LOCK);
    }
    return BASE_STATUS_SUCCESS;
}

BASE_DECLARE(base_xml_t) base_xml_open_root(uint8_t reload, const char **err)
{
    base_xml_t root = NULL;
    base_event_t *event;

    base_mutex_lock(XML_LOCK);

    if (XML_OPEN_ROOT_FUNCTION) {
        root = XML_OPEN_ROOT_FUNCTION(reload, err, XML_OPEN_ROOT_FUNCTION_USER_DATA);
    }
    base_mutex_unlock(XML_LOCK);


    if (root) {
        if (base_event_create(&event, BASE_EVENT_RELOADXML) == BASE_STATUS_SUCCESS) {
            if (base_event_fire(&event) != BASE_STATUS_SUCCESS) {
                base_event_destroy(&event);
            }
        }
    }

    return root;
}

BASE_DECLARE_NONSTD(base_xml_t) __base_xml_open_root(uint8_t reload, const char **err, void *user_data)
{
    char path_buf[1024];
    uint8_t errcnt = 0;
    base_xml_t new_main, r = NULL;

    if (MAIN_XML_ROOT) {
        if (!reload) {
            r = base_xml_root();
            goto done;
        }
    }

    base_snprintf(path_buf, sizeof(path_buf), "%s%s%s", BASE_GLOBAL_dirs.conf_dir, BASE_PATH_SEPARATOR, BASE_GLOBAL_filenames.conf_name);
    if ((new_main = base_xml_parse_file(path_buf))) {
        *err = base_xml_error(new_main);
        base_copy_string(not_so_threadsafe_error_buffer, *err, sizeof(not_so_threadsafe_error_buffer));
        *err = not_so_threadsafe_error_buffer;
        if (!zstr(*err)) {
            base_xml_free(new_main);
            new_main = NULL;
            errcnt++;
        } else {
            *err = "Success";
            base_xml_set_root(new_main);

        }
    } else {
        *err = "Cannot Open log directory or XML Root!";
        errcnt++;
    }

    if (errcnt == 0) {
        r = base_xml_root();
    }

done:

    return r;
}

BASE_DECLARE(base_status_t) base_xml_reload(const char **err)
{
    base_xml_t xml_root;

    if ((xml_root = base_xml_open_root(1, err))) {
        base_xml_free(xml_root);
        return BASE_STATUS_SUCCESS;
    }

    return BASE_STATUS_GENERR;
}

BASE_DECLARE(base_status_t) base_xml_init(base_memory_pool_t *pool, const char **err)
{
    base_xml_t xml;
    XML_MEMORY_POOL = pool;
    *err = "Success";

    base_mutex_init(&CACHE_MUTEX, BASE_MUTEX_NESTED, XML_MEMORY_POOL);
    base_mutex_init(&XML_LOCK, BASE_MUTEX_NESTED, XML_MEMORY_POOL);
    base_mutex_init(&REFLOCK, BASE_MUTEX_NESTED, XML_MEMORY_POOL);
    base_mutex_init(&FILE_LOCK, BASE_MUTEX_NESTED, XML_MEMORY_POOL);
    base_core_hash_init(&CACHE_HASH);
    base_core_hash_init(&CACHE_EXPIRES_HASH);

    base_thread_rwlock_create(&B_RWLOCK, XML_MEMORY_POOL);

    assert(pool != NULL);

    if ((xml = base_xml_open_root(FALSE, err))) {
        base_xml_free(xml);
        return BASE_STATUS_SUCCESS;
    } else {
        return BASE_STATUS_FALSE;
    }
}

BASE_DECLARE(base_status_t) base_xml_destroy(void)
{
    base_status_t status = BASE_STATUS_FALSE;


    base_mutex_lock(XML_LOCK);
    base_mutex_lock(REFLOCK);

    if (MAIN_XML_ROOT) {
        base_xml_t xml = MAIN_XML_ROOT;
        MAIN_XML_ROOT = NULL;
        base_xml_free(xml);
        status = BASE_STATUS_SUCCESS;
    }

    base_mutex_unlock(XML_LOCK);
    base_mutex_unlock(REFLOCK);

    base_xml_clear_user_cache(NULL, NULL, NULL);

    base_core_hash_destroy(&CACHE_HASH);
    base_core_hash_destroy(&CACHE_EXPIRES_HASH);

    return status;
}

BASE_DECLARE(base_xml_t) base_xml_open_cfg(const char *file_path, base_xml_t *node, base_event_t *params)
{
    base_xml_t xml = NULL, cfg = NULL;

    *node = NULL;

    assert(MAIN_XML_ROOT != NULL);

    if (base_xml_locate("configuration", "configuration", "name", file_path, &xml, &cfg, params, BASE_FALSE) == BASE_STATUS_SUCCESS) {
        *node = cfg;
    }

    return xml;

}

/* Encodes ampersand sequences appending the results to *dst, reallocating *dst
if length exceeds max. a is non-zero for attribute encoding. Returns *dst */
static char *base_xml_ampencode(const char *s, base_size_t len, char **dst, base_size_t *dlen, base_size_t *max, short a, base_bool_t use_utf8_encoding)
{
    const char *e = NULL;
    int immune = 0;
    int expecting_x_utf_8_char = 0;
    int unicode_char = 0x000000;

    if (!(s && *s))
        return *dst;

    if (len) {
        e = s + len;
    }

    while (s != e) {
        while (*dlen + 10 > *max) {
            *dst = (char *) base_must_realloc(*dst, *max += BASE_XML_BUFSIZE);
        }

        if (immune) {
            if (*s == '\0') {
                return *dst;
            }
            (*dst)[(*dlen)++] = *s;
        } else
            switch (*s) {
            case '\0':
                return *dst;
            case '&':
                *dlen += sprintf(*dst + *dlen, "&amp;");
                break;
            case '<':
                if (*(s + 1) == '!') {
                    (*dst)[(*dlen)++] = *s;
                    immune++;
                    break;
                }
                *dlen += sprintf(*dst + *dlen, "&lt;");
                break;
            case '>':
                *dlen += sprintf(*dst + *dlen, "&gt;");
                break;
            case '"':
                *dlen += sprintf(*dst + *dlen, (a) ? "&quot;" : "\"");
                break;
            case '\n':
                *dlen += sprintf(*dst + *dlen, (a) ? "&#xA;" : "\n");
                break;
            case '\t':
                *dlen += sprintf(*dst + *dlen, (a) ? "&#x9;" : "\t");
                break;
            case '\r':
                *dlen += sprintf(*dst + *dlen, "&#xD;");
                break;
            default:
                if (use_utf8_encoding && expecting_x_utf_8_char == 0 && ((*s >> 8) & 0x01)) {
                    int num = 1;
                    for (;num<4;num++) {
                        if (! ((*s >> (7-num)) & 0x01)) {
                            break;
                        }
                    }
                    switch (num) {
                    case 2:
                        unicode_char = *s & 0x1f;
                        break;
                    case 3:
                        unicode_char = *s & 0x0f;
                        break;
                    case 4:
                        unicode_char = *s & 0x07;
                        break;
                    default:
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Invalid UTF-8 Initial charactere, skip it\n");
                        /* ERROR HERE */
                        break;
                    }
                    expecting_x_utf_8_char = num - 1;

                } else if (use_utf8_encoding && expecting_x_utf_8_char > 0) {
                    if (((*s >> 6) & 0x03) == 0x2) {

                        unicode_char = unicode_char << 6;
                        unicode_char = unicode_char | (*s & 0x3f);
                    } else {
                        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Invalid UTF-8 character to ampersand, skip it\n");
                        expecting_x_utf_8_char = 0;
                        break;
                    }
                    expecting_x_utf_8_char--;
                    if (expecting_x_utf_8_char == 0) {
                        *dlen += sprintf(*dst + *dlen, "&#x%X;", unicode_char);
                    }
                } else {
                    (*dst)[(*dlen)++] = *s;
                }
            }
        s++;
    }
    return *dst;
}

#define XML_INDENT "  "
/* Recursively converts each tag to xml appending it to *s. Reallocates *s if
its length exceeds max. start is the location of the previous tag in the
parent tag's character content. Returns *s. */
static char *base_xml_toxml_r(base_xml_t xml, char **s, base_size_t *len, base_size_t *max, base_size_t start, char ***attr, uint32_t *count, int isroot, base_bool_t use_utf8_encoding)
{
    int i, j;
    char *txt;
    base_size_t off;
    uint32_t lcount;
    uint32_t loops = 0;

tailrecurse:
    off = 0;
    txt = "";

    if (loops++) {
        isroot = 0;
    }

    if (!isroot && xml->parent) {
        txt = (char *) xml->parent->txt;
    }

    /* parent character content up to this tag */
    *s = base_xml_ampencode(txt + start, xml->off - start, s, len, max, 0, use_utf8_encoding);

    while (*len + strlen(xml->name) + 5 + (strlen(XML_INDENT) * (*count)) + 1 > *max) {	/* reallocate s */
        *s = (char *) base_must_realloc(*s, *max += BASE_XML_BUFSIZE);
    }

    if (*len && *(*s + (*len) - 1) == '>') {
        *len += sprintf(*s + *len, "\n");	/* indent */
    }
    for (lcount = 0; lcount < *count; lcount++) {
        *len += sprintf(*s + *len, "%s", XML_INDENT);	/* indent */
    }

    *len += sprintf(*s + *len, "<%s", xml->name);	/* open tag */
    for (i = 0; xml->attr[i]; i += 2) {	/* tag attributes */
        if (base_xml_attr(xml, xml->attr[i]) != xml->attr[i + 1])
            continue;
        while (*len + strlen(xml->attr[i]) + 7 + (strlen(XML_INDENT) * (*count)) > *max) {	/* reallocate s */
            *s = (char *) base_must_realloc(*s, *max += BASE_XML_BUFSIZE);
        }

        *len += sprintf(*s + *len, " %s=\"", xml->attr[i]);
        base_xml_ampencode(xml->attr[i + 1], 0, s, len, max, 1, use_utf8_encoding);
        *len += sprintf(*s + *len, "\"");
    }

    for (i = 0; attr[i] && strcmp(attr[i][0], xml->name); i++);
    for (j = 1; attr[i] && attr[i][j]; j += 3) {	/* default attributes */
        if (!attr[i][j + 1] || base_xml_attr(xml, attr[i][j]) != attr[i][j + 1])
            continue;			/* skip duplicates and non-values */
        while (*len + strlen(attr[i][j]) + 8 + (strlen(XML_INDENT) * (*count)) > *max) {	/* reallocate s */
            *s = (char *) base_must_realloc(*s, *max += BASE_XML_BUFSIZE);
        }

        *len += sprintf(*s + *len, " %s=\"", attr[i][j]);
        base_xml_ampencode(attr[i][j + 1], 0, s, len, max, 1, use_utf8_encoding);
        *len += sprintf(*s + *len, "\"");
    }

    *len += sprintf(*s + *len, (xml->child || xml->txt) ? ">" : "/>\n");

    if (xml->child) {
        (*count)++;
        *s = base_xml_toxml_r(xml->child, s, len, max, 0, attr, count, 0, use_utf8_encoding);

    } else {
        *s = base_xml_ampencode(xml->txt, 0, s, len, max, 0, use_utf8_encoding);	/* data */
    }

    while (*len + strlen(xml->name) + 5 + (strlen(XML_INDENT) * (*count)) > *max) {	/* reallocate s */
        *s = (char *) base_must_realloc(*s, *max += BASE_XML_BUFSIZE);
    }

    if (xml->child || xml->txt) {
        if (*(*s + (*len) - 1) == '\n') {
            for (lcount = 0; lcount < *count; lcount++) {
                *len += sprintf(*s + *len, "%s", XML_INDENT);	/* indent */
            }
        }
        *len += sprintf(*s + (*len), "</%s>\n", xml->name);	/* close tag */
    }

    while (txt[off] && off < xml->off)
        off++;					/* make sure off is within bounds */

    if (!isroot && xml->ordered) {
        xml = xml->ordered;
        start = off;
        goto tailrecurse;
        /*
        return base_xml_toxml_r(xml->ordered, s, len, max, off, attr, count, use_utf8_encoding);
        */
    } else {
        if (*count > 0)
            (*count)--;
        return base_xml_ampencode(txt + off, 0, s, len, max, 0, use_utf8_encoding);
    }
}

BASE_DECLARE(char *) base_xml_toxml_nolock_ex(base_xml_t xml, base_bool_t prn_header, base_bool_t use_utf8_encoding)
{
    char *s = (char *) base_must_malloc(BASE_XML_BUFSIZE);

    return base_xml_toxml_buf_ex(xml, s, BASE_XML_BUFSIZE, 0, prn_header, use_utf8_encoding);
}

BASE_DECLARE(char *) base_xml_toxml_ex(base_xml_t xml, base_bool_t prn_header, base_bool_t use_utf8_encoding)
{
    char *r, *s;

    s = (char *) base_must_malloc(BASE_XML_BUFSIZE);

    r = base_xml_toxml_buf_ex(xml, s, BASE_XML_BUFSIZE, 0, prn_header, use_utf8_encoding);

    return r;
}

BASE_DECLARE(char *) base_xml_tohtml_ex(base_xml_t xml, base_bool_t prn_header, base_bool_t use_utf8_encoding)
{
    char *r, *s, *h;
    base_size_t rlen = 0;
    base_size_t len = BASE_XML_BUFSIZE;

    s = (char *) base_must_malloc(BASE_XML_BUFSIZE);
    h = (char *) base_must_malloc(BASE_XML_BUFSIZE);

    r = base_xml_toxml_buf_ex(xml, s, BASE_XML_BUFSIZE, 0, prn_header, use_utf8_encoding);
    h = base_xml_ampencode(r, 0, &h, &rlen, &len, 1, use_utf8_encoding);
    base_safe_free(r);
    return h;
}

/* converts a base_xml structure back to xml, returning a string of xml data that
must be freed */
BASE_DECLARE(char *) base_xml_toxml_buf_ex(base_xml_t xml, char *buf, base_size_t buflen, base_size_t offset, base_bool_t prn_header, base_bool_t use_utf8_encoding)
{
    base_xml_t p = (xml) ? xml->parent : NULL;
    base_xml_root_t root = (base_xml_root_t) xml;
    base_size_t len = 0, max = buflen;
    char *s, *t, *n;
    int i, j, k;
    uint32_t count = 0;

    s = buf;
    assert(s != NULL);
    memset(s, 0, max);
    len += offset;
    if (prn_header) {
        len += sprintf(s + len, "<?xml version=\"1.0\"?>\n");
    }

    if (!xml || !xml->name) {
        return (char *) base_must_realloc(s, len + 1);
    }

    while (root->xml.parent) {
        root = (base_xml_root_t) root->xml.parent;	/* root tag */
    }

    for (i = 0; !p && root->pi[i]; i++) {	/* pre-root processing instructions */
        for (k = 2; root->pi[i][k - 1]; k++);
        for (j = 1; (n = root->pi[i][j]); j++) {
            if (root->pi[i][k][j - 1] == '>') {
                continue;		/* not pre-root */
            }
            while (len + strlen(t = root->pi[i][0]) + strlen(n) + 7 > max) {
                s = (char *) base_must_realloc(s, max += BASE_XML_BUFSIZE);
            }
            len += sprintf(s + len, "<?%s%s%s?>", t, *n ? " " : "", n);
        }
    }

    s = base_xml_toxml_r(xml, &s, &len, &max, 0, root->attr, &count, 1, use_utf8_encoding);

    for (i = 0; !p && root->pi[i]; i++) {	/* post-root processing instructions */
        for (k = 2; root->pi[i][k - 1]; k++);
        for (j = 1; (n = root->pi[i][j]); j++) {
            if (root->pi[i][k][j - 1] == '<') {
                continue;		/* not post-root */
            }
            while (len + strlen(t = root->pi[i][0]) + strlen(n) + 7 > max) {
                s = (char *) base_must_realloc(s, max += BASE_XML_BUFSIZE);
            }
            len += sprintf(s + len, "\n<?%s%s%s?>", t, *n ? " " : "", n);
        }
    }

    return (char *) base_must_realloc(s, len + 1);
}

/* free the memory allocated for the base_xml structure */
BASE_DECLARE(void) base_xml_free(base_xml_t xml)
{
    base_xml_root_t root;
    int i, j;
    char **a, *s;
    base_xml_t orig_xml;
    int refs = 0;

tailrecurse:
    root = (base_xml_root_t) xml;
    if (!xml) {
        return;
    }

    if (base_test_flag(xml, BASE_XML_ROOT)) {
        base_mutex_lock(REFLOCK);

        if (xml->refs) {
            xml->refs--;
            refs = xml->refs;
        }
        base_mutex_unlock(REFLOCK);
    }

    if (refs) {
        return;
    }

    if (xml->free_path) {
        if (unlink(xml->free_path) != 0) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING, "Failed to delete file [%s]\n", xml->free_path);
        }
        base_safe_free(xml->free_path);
    }

    base_xml_free(xml->child);
    /*base_xml_free(xml->ordered); */

    if (!xml->parent) {			/* free root tag allocations */
#if (_MSC_VER >= 1400)			// VC8+
        __analysis_assume(sizeof(root->ent) > 44);	/* tail recursion confuses code analysis */
#endif
        for (i = 10; root->ent[i]; i += 2)	/* 0 - 9 are default entities (<>&"') */
            if ((s = root->ent[i + 1]) < root->s || s > root->e)
                free(s);
        free(root->ent);		/* free list of general entities */

        for (i = 0; (a = root->attr[i]); i++) {
            for (j = 1; a[j++]; j += 2)	/* free malloced attribute values */
                if (a[j] && (a[j] < root->s || a[j] > root->e))
                    free(a[j]);
            free(a);
        }
        if (root->attr[0])
            free(root->attr);	/* free default attribute list */

        for (i = 0; root->pi[i]; i++) {
            for (j = 1; root->pi[i][j]; j++);
            free(root->pi[i][j + 1]);
            free(root->pi[i]);
        }
        if (root->pi[0])
            free(root->pi);		/* free processing instructions */

        if (root->dynamic == 1)
            free(root->m);		/* malloced xml data */
        if (root->u)
            free(root->u);		/* utf8 conversion */
    }

    base_xml_free_attr(xml->attr);	/* tag attributes */
    if ((xml->flags & BASE_XML_TXTM))
        free(xml->txt);			/* character content */
    if ((xml->flags & BASE_XML_NAMEM))
        free(xml->name);		/* tag name */
    if (xml->ordered) {
        orig_xml = xml;
        xml = xml->ordered;
        free(orig_xml);
        goto tailrecurse;
    }
    free(xml);
}

/* return parser error message or empty string if none */
BASE_DECLARE(const char *) base_xml_error(base_xml_t xml)
{
    while (xml && xml->parent)
        xml = xml->parent;		/* find root tag */
    return (xml) ? ((base_xml_root_t) xml)->err : "";
}

/* returns a new empty base_xml structure with the given root tag name */
BASE_DECLARE(base_xml_t) base_xml_new(const char *name)
{
    static const char *ent[] = { "lt;", "&#60;", "gt;", "&#62;", "quot;", "&#34;",
        "apos;", "&#39;", "amp;", "&#38;", NULL
    };
    base_xml_root_t root = (base_xml_root_t) base_must_malloc(sizeof(struct base_xml_root));

    memset(root, '\0', sizeof(struct base_xml_root));
    root->xml.name = (char *) name;
    root->cur = &root->xml;
    strcpy(root->err, root->xml.txt = (char *) "");
    root->ent = (char **) memcpy(base_must_malloc(sizeof(ent)), ent, sizeof(ent));
    root->attr = root->pi = (char ***) (root->xml.attr = BASE_XML_NIL);
    return &root->xml;
}

/* inserts an existing tag into a base_xml structure */
BASE_DECLARE(base_xml_t) base_xml_insert(base_xml_t xml, base_xml_t dest, base_size_t off)
{
    base_xml_t cur, prev, head;

    xml->next = xml->sibling = xml->ordered = NULL;
    xml->off = off;
    xml->parent = dest;

    if ((head = dest->child)) {	/* already have sub tags */
        if (head->off <= off) {	/* not first subtag */
            for (cur = head; cur->ordered && cur->ordered->off <= off; cur = cur->ordered);
            xml->ordered = cur->ordered;
            cur->ordered = xml;
        } else {				/* first subtag */
            xml->ordered = head;
            dest->child = xml;
        }

        for (cur = head, prev = NULL; cur && strcmp(cur->name, xml->name); prev = cur, cur = cur->sibling);	/* find tag type */
        if (cur && cur->off <= off) {	/* not first of type */
            while (cur->next && cur->next->off <= off)
                cur = cur->next;
            xml->next = cur->next;
            cur->next = xml;
        } else {				/* first tag of this type */
            if (prev && cur)
                prev->sibling = cur->sibling;	/* remove old first */
            xml->next = cur;	/* old first tag is now next */
            for (cur = head, prev = NULL; cur && cur->off <= off; prev = cur, cur = cur->sibling);	/* new sibling insert point */
            xml->sibling = cur;
            if (prev)
                prev->sibling = xml;
        }
    } else
        dest->child = xml;		/* only sub tag */

    return xml;
}

/* Adds a child tag. off is the offset of the child tag relative to the start
of the parent tag's character content. Returns the child tag */
BASE_DECLARE(base_xml_t) base_xml_add_child(base_xml_t xml, const char *name, base_size_t off)
{
    base_xml_t child;

    if (!xml)
        return NULL;
    child = (base_xml_t) base_must_malloc(sizeof(struct base_xml));

    memset(child, '\0', sizeof(struct base_xml));
    child->name = (char *) name;
    child->attr = BASE_XML_NIL;
    child->off = off;
    child->parent = xml;
    child->txt = (char *) "";

    return base_xml_insert(child, xml, off);
}

/* Adds a child tag. off is the offset of the child tag relative to the start
of the parent tag's character content. Returns the child tag */
BASE_DECLARE(base_xml_t) base_xml_add_child_d(base_xml_t xml, const char *name, base_size_t off)
{
    if (!xml) return NULL;
    return base_xml_set_flag(base_xml_add_child(xml, strdup(name), off), BASE_XML_NAMEM);
}

/* sets the character content for the given tag and returns the tag */
BASE_DECLARE(base_xml_t) base_xml_set_txt(base_xml_t xml, const char *txt)
{
    if (!xml)
        return NULL;
    if (xml->flags & BASE_XML_TXTM)
        free(xml->txt);			/* existing txt was malloced */
    xml->flags &= ~BASE_XML_TXTM;
    xml->txt = (char *) txt;
    return xml;
}

/* sets the character content for the given tag and returns the tag */
BASE_DECLARE(base_xml_t) base_xml_set_txt_d(base_xml_t xml, const char *txt)
{
    if (!xml) return NULL;
    return base_xml_set_flag(base_xml_set_txt(xml, strdup(txt)), BASE_XML_TXTM);
}

/* Sets the given tag attribute or adds a new attribute if not found. A value
of NULL will remove the specified attribute.  Returns the tag given */
BASE_DECLARE(base_xml_t) base_xml_set_attr(base_xml_t xml, const char *name, const char *value)
{
    int l = 0, c;

    if (!xml)
        return NULL;
    while (xml->attr[l] && strcmp(xml->attr[l], name))
        l += 2;
    if (!xml->attr[l]) {		/* not found, add as new attribute */
        if (!value)
            return xml;			/* nothing to do */
        if (xml->attr == BASE_XML_NIL) {	/* first attribute */
            xml->attr = (char **) base_must_malloc(4 * sizeof(char *));
            xml->attr[l + 1] = base_must_strdup("");	/* empty list of malloced names/vals */
        } else {
            xml->attr = (char **) base_must_realloc(xml->attr, (l + 4) * sizeof(char *));
        }

        xml->attr[l] = (char *) name;	/* set attribute name */
        xml->attr[l + 2] = NULL;	/* null terminate attribute list */
        xml->attr[l + 3] = (char *) base_must_realloc(xml->attr[l + 1], (c = (int) strlen(xml->attr[l + 1])) + 2);
        strcpy(xml->attr[l + 3] + c, " ");	/* set name/value as not malloced */
        if (xml->flags & BASE_XML_DUP)
            xml->attr[l + 3][c] = BASE_XML_NAMEM;
        c = l + 2; /* end of attribute list */
    } else {
        for (c = l; xml->attr[c]; c += 2);	/* find end of attribute list */

        if (xml->flags & BASE_XML_DUP)
            free((char*)name);	/* name was strduped */
        if (xml->attr[c + 1][l / 2] & BASE_XML_TXTM)
            free(xml->attr[l + 1]);	/* old val */
    }

    if (xml->flags & BASE_XML_DUP)
        xml->attr[c + 1][l / 2] |= BASE_XML_TXTM;
    else
        xml->attr[c + 1][l / 2] &= ~BASE_XML_TXTM;

    if (value)
        xml->attr[l + 1] = (char *) value;	/* set attribute value */
    else {						/* remove attribute */
        if (xml->attr[c + 1][l / 2] & BASE_XML_NAMEM)
            free(xml->attr[l]);
        c -= 2;
        if (c > 0) {
            memmove(xml->attr + l, xml->attr + l + 2, (c - l + 2) * sizeof(char*));
            xml->attr = (char**)base_must_realloc(xml->attr, (c + 2) * sizeof(char*));
            memmove(xml->attr[c + 1] + (l / 2), xml->attr[c + 1] + (l / 2) + 1, (c / 2) - (l / 2));	/* fix list of which name/vals are malloced */
            xml->attr[c + 1][c / 2] = '\0';
        } else {
            /* last attribute removed, reset attribute list */
            free(xml->attr[3]);
            free(xml->attr);
            xml->attr = BASE_XML_NIL;
        }
    }
    xml->flags &= ~BASE_XML_DUP;	/* clear strdup() flag */

    return xml;
}

/* Sets the given tag attribute or adds a new attribute if not found. A value
of NULL will remove the specified attribute.  Returns the tag given */
BASE_DECLARE(base_xml_t) base_xml_set_attr_d(base_xml_t xml, const char *name, const char *value)
{
    if (!xml) return NULL;
    return base_xml_set_attr(base_xml_set_flag(xml, BASE_XML_DUP), base_must_strdup(name), base_must_strdup(base_str_nil(value)));
}

/* Sets the given tag attribute or adds a new attribute if not found. A value
of NULL will remove the specified attribute.  Returns the tag given */
BASE_DECLARE(base_xml_t) base_xml_set_attr_d_buf(base_xml_t xml, const char *name, const char *value)
{
    if (!xml) return NULL;
    return base_xml_set_attr(base_xml_set_flag(xml, BASE_XML_DUP), base_must_strdup(name), base_must_strdup(value));
}

/* sets a flag for the given tag and returns the tag */
BASE_DECLARE(base_xml_t) base_xml_set_flag(base_xml_t xml, base_xml_flag_t flag)
{
    if (xml)
        xml->flags |= flag;
    return xml;
}

/* removes a tag along with its subtags without freeing its memory */
BASE_DECLARE(base_xml_t) base_xml_cut(base_xml_t xml)
{
    base_xml_t cur;

    if (!xml)
        return NULL;			/* nothing to do */
    if (xml->next)
        xml->next->sibling = xml->sibling;	/* patch sibling list */

    if (xml->parent) {			/* not root tag */
        cur = xml->parent->child;	/* find head of subtag list */
        if (cur == xml)
            xml->parent->child = xml->ordered;	/* first subtag */
        else {					/* not first subtag */
            while (cur->ordered != xml)
                cur = cur->ordered;
            cur->ordered = cur->ordered->ordered;	/* patch ordered list */

            cur = xml->parent->child;	/* go back to head of subtag list */
            if (strcmp(cur->name, xml->name)) {	/* not in first sibling list */
                while (strcmp(cur->sibling->name, xml->name))
                    cur = cur->sibling;
                if (cur->sibling == xml) {	/* first of a sibling list */
                    cur->sibling = (xml->next) ? xml->next : cur->sibling->sibling;
                } else
                    cur = cur->sibling;	/* not first of a sibling list */
            }

            while (cur->next && cur->next != xml)
                cur = cur->next;
            if (cur->next)
                cur->next = cur->next->next;	/* patch next list */
        }
    }
    xml->ordered = xml->sibling = xml->next = NULL;	/* prevent base_xml_free() from clobbering ordered list */
    return xml;
}

BASE_DECLARE(int) base_xml_std_datetime_check(base_xml_t xcond, int *offset, const char *tzname)
{

    const char *xdt = base_xml_attr(xcond, "date-time");
    const char *xyear = base_xml_attr(xcond, "year");
    const char *xyday = base_xml_attr(xcond, "yday");
    const char *xmon = base_xml_attr(xcond, "mon");
    const char *xmday = base_xml_attr(xcond, "mday");
    const char *xweek = base_xml_attr(xcond, "week");
    const char *xmweek = base_xml_attr(xcond, "mweek");
    const char *xwday = base_xml_attr(xcond, "wday");
    const char *xhour = base_xml_attr(xcond, "hour");
    const char *xminute = base_xml_attr(xcond, "minute");
    const char *xminday = base_xml_attr(xcond, "minute-of-day");
    const char *xtod = base_xml_attr(xcond, "time-of-day");
    const char *tzoff = base_xml_attr(xcond, "tz-offset");
    const char *isdst = base_xml_attr(xcond, "dst");

    int loffset = -1000;
    int eoffset = -1000;
    int dst = -1000;
    base_time_t ts = base_micro_time_now();
    int time_match = -1;
    base_time_exp_t tm, tm2;

    if (!zstr(isdst)) {
        dst = base_true(isdst);
    }

    if (!zstr(tzoff) && base_is_number(tzoff)) {
        loffset = atoi(tzoff);
    }

    base_time_exp_lt(&tm2, ts);

    if (offset) {
        eoffset = *offset;
        base_time_exp_tz(&tm, ts, *offset * 3600);
    } else if (!zstr(tzname)) {
        base_time_exp_tz_name(tzname, &tm, ts);
    } else {
        tm = tm2;
    }

    if (eoffset == -1000) {
        eoffset = tm.tm_gmtoff / 3600;
    }

    if (loffset == -1000) {
        loffset = eoffset;
    }


    if (time_match && tzoff) {
        time_match = loffset == eoffset;
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: TZOFFSET[%d] == %d (%s)\n", eoffset, loffset, time_match ? "PASS" : "FAIL");

    }

    if (time_match && dst > -1) {
        time_match = (tm2.tm_isdst > 0 && dst > 0);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: DST[%s] == %s (%s)\n",
            tm2.tm_isdst > 0 ? "true" : "false", dst > 0 ? "true" : "false", time_match ? "PASS" : "FAIL");

    }

    if (time_match && xdt) {
        char tmpdate[80];
        base_size_t retsize;
        base_strftime(tmpdate, &retsize, sizeof(tmpdate), "%Y-%m-%d %H:%M:%S", &tm);
        time_match = base_fulldate_cmp(xdt, &ts);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG,
            "XML DateTime Check: date time[%s] =~ %s (%s)\n", tmpdate, xdt, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xyear) {
        int test = tm.tm_year + 1900;
        time_match = base_number_cmp(xyear, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: year[%d] =~ %s (%s)\n", test, xyear, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xyday) {
        int test = tm.tm_yday + 1;
        time_match = base_number_cmp(xyday, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: day of year[%d] =~ %s (%s)\n", test, xyday, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xmon) {
        int test = tm.tm_mon + 1;
        time_match = base_number_cmp(xmon, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: month[%d] =~ %s (%s)\n", test, xmon, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xmday) {
        int test = tm.tm_mday;
        time_match = base_number_cmp(xmday, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: day of month[%d] =~ %s (%s)\n", test, xmday, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xweek) {
        int test = (int) (tm.tm_yday / 7 + 1);
        time_match = base_number_cmp(xweek, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: week of year[%d] =~ %s (%s)\n", test, xweek, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xmweek) {
        /* calculate the day of the week of the first of the month (0-6) */
        int firstdow = (int) (7 - (tm.tm_mday - (tm.tm_wday + 1)) % 7) % 7;
        /* calculate the week of the month (1-6)*/
        int test = (int) ceil((tm.tm_mday + firstdow) / 7.0);
        time_match = base_number_cmp(xmweek, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime: week of month[%d] =~ %s (%s)\n", test, xmweek, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xwday) {
        int test = tm.tm_wday + 1;
        time_match = base_dow_cmp(xwday, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: day of week[%s] =~ %s (%s)\n", base_dow_int2str(test), xwday, time_match ? "PASS" : "FAIL");
    }
    if (time_match && xhour) {
        int test = tm.tm_hour;
        time_match = base_number_cmp(xhour, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: hour[%d] =~ %s (%s)\n", test, xhour, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xminute) {
        int test = tm.tm_min;
        time_match = base_number_cmp(xminute, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: minute[%d] =~ %s (%s)\n", test, xminute, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xminday) {
        int test = (tm.tm_hour * 60) + (tm.tm_min + 1);
        time_match = base_number_cmp(xminday, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: minute of day[%d] =~ %s (%s)\n", test, xminday, time_match ? "PASS" : "FAIL");
    }

    if (time_match && xtod) {
        int test = (tm.tm_hour * 60 * 60) + (tm.tm_min * 60) + tm.tm_sec;
        char tmpdate[10];
        base_snprintf(tmpdate, 10, "%d:%d:%d", tm.tm_hour, tm.tm_min, tm.tm_sec);
        time_match = base_tod_cmp(xtod, test);
        base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_DEBUG9,
            "XML DateTime Check: time of day[%s] =~ %s (%s)\n", tmpdate, xtod, time_match ? "PASS" : "FAIL");
    }

    return time_match;
}

BASE_DECLARE(base_status_t) base_xml_locate_language_ex(base_xml_t *root, base_xml_t *node, base_event_t *params, base_xml_t *language, base_xml_t *phrases, base_xml_t *macros, const char *str_language) {
    base_status_t status = BASE_STATUS_FALSE;

    if (base_xml_locate("languages", NULL, NULL, NULL, root, node, params, BASE_TRUE) != BASE_STATUS_SUCCESS) {
        base_xml_t sub_macros;

        if (base_xml_locate("phrases", NULL, NULL, NULL, root, node, params, BASE_TRUE) != BASE_STATUS_SUCCESS) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Open of languages and phrases failed.\n");
            goto done;
        }
        if (!(sub_macros = base_xml_child(*node, "macros"))) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Can't find macros tag.\n");
            base_xml_free(*root);
            *root = NULL;
            *node = NULL;
            goto done;
        }
        if (!(*language = base_xml_find_child(sub_macros, "language", "name", str_language))) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Can't find language %s.\n", str_language);
            base_xml_free(*root);
            *root = NULL;
            *node = NULL;
            goto done;
        }
        *macros = *language;
    } else {
        if (!(*language = base_xml_find_child(*node, "language", "name", str_language))) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Can't find language %s.\n", str_language);
            base_xml_free(*root);
            *root = NULL;
            goto done;
        }
        if (!(*phrases = base_xml_child(*language, "phrases"))) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Can't find phrases tag.\n");
            base_xml_free(*root);
            *root = NULL;
            *node = NULL;
            *language = NULL;
            goto done;
        }

        if (!(*macros = base_xml_child(*phrases, "macros"))) {
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_ERROR, "Can't find macros tag.\n");
            base_xml_free(*root);
            *root = NULL;
            *node = NULL;
            *language = NULL;
            *phrases = NULL;
            goto done;
        }
    }
    status = BASE_STATUS_SUCCESS;

done:
    return status;
}

BASE_DECLARE(base_status_t) base_xml_locate_language(base_xml_t *root, base_xml_t *node, base_event_t *params, base_xml_t *language, base_xml_t *phrases, base_xml_t *macros, const char *str_language) {
    base_status_t status;

    if ((status = base_xml_locate_language_ex(root, node, params, language, phrases, macros, str_language)) != BASE_STATUS_SUCCESS) {
        char *str_language_dup = strdup(str_language);
        char *secondary;
        base_assert(str_language_dup);
        if ((secondary = strchr(str_language_dup, '-'))) {
            *secondary++ = '\0';
            base_log_printf(BASE_CHANNEL_LOG, BASE_LOG_WARNING,
                "language %s not found. trying %s by removing %s\n", str_language, str_language_dup, secondary);
            base_event_add_header_string(params, BASE_STACK_BOTTOM, "lang", str_language_dup);
            status = base_xml_locate_language_ex(root, node, params, language, phrases, macros, str_language_dup);
        }
        base_safe_free(str_language_dup);
    }

    return status;
}

#ifdef WIN32
/*
* globbing functions for windows, part of libc on unix, this code was cut and paste from
* freebsd lib and distilled a bit to work with windows
*/

/*
* Copyright (c) 1989, 1993
*	The Regents of the University of California.  All rights reserved.
*
* This code is derived from software contributed to Berkeley by
* Guido van Rossum.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 4. Neither the name of the University nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#define	DOLLAR		'$'
#define	DOT		'.'
#define	EOS		'\0'
#define	LBRACKET	'['
#define	NOT		'!'
#define	QUESTION	'?'
#define	RANGE		'-'
#define	RBRACKET	']'
#define	SEP		'/'
#define WIN_SEP '/'
#define	STAR		'*'
#define	TILDE		'~'
#define	UNDERSCORE	'_'
#define	LBRACE		'{'
#define	RBRACE		'}'
#define	SLASH		'/'
#define	COMMA		','

#define	M_QUOTE		(char)0x80
#define	M_PROTECT	(char)0x40
#define	M_MASK		(char)0xff
#define	M_ASCII		(char)0x7f

#define	CHAR(c)		((char)((c)&M_ASCII))
#define	META(c)		((char)((c)|M_QUOTE))
#define	M_ALL		META('*')
#define	M_END		META(']')
#define	M_NOT		META('!')
#define	M_ONE		META('?')
#define	M_RNG		META('-')
#define	M_SET		META('[')
#define	ismeta(c)	(((c)&M_QUOTE) != 0)

#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

static int compare(const void *, const void *);
static int glob0(const char *, glob_t *, size_t *);
static int glob1(char *, glob_t *, size_t *);
static int glob2(char *, char *, char *, char *, glob_t *, size_t *);
static int glob3(char *, char *, char *, char *, char *, glob_t *, size_t *);
static int globextend(const char *, glob_t *, size_t *);
static int match(char *, char *, char *);

#pragma warning(push)
#pragma warning(disable:4310)

int glob(const char *pattern, int flags, int (*errfunc) (const char *, int), glob_t *pglob)
{
    const unsigned char *patnext;
    size_t limit;
    char c;
    char *bufnext, *bufend, patbuf[MAXPATHLEN];

    patnext = (unsigned char *) pattern;
    if (!(flags & GLOB_APPEND)) {
        pglob->gl_pathc = 0;
        pglob->gl_pathv = NULL;
        if (!(flags & GLOB_DOOFFS))
            pglob->gl_offs = 0;
    }
    if (flags & GLOB_LIMIT) {
        limit = pglob->gl_matchc;
        if (limit == 0)
            limit = 9999999;
    } else
        limit = 0;
    pglob->gl_flags = flags & ~GLOB_MAGCHAR;
    pglob->gl_errfunc = errfunc;
    pglob->gl_matchc = 0;

    bufnext = patbuf;
    bufend = bufnext + MAXPATHLEN - 1;
    while (bufnext < bufend && (c = *patnext++) != EOS)
        *bufnext++ = c;
    *bufnext = EOS;

    return glob0(patbuf, pglob, &limit);
}

/*
* The main glob() routine: compiles the pattern (optionally processing
* quotes), calls glob1() to do the real pattern matching, and finally
* sorts the list (unless unsorted operation is requested).  Returns 0
* if things went well, nonzero if errors occurred.
*/
static int glob0(const char *pattern, glob_t *pglob, size_t *limit)
{
    const char *qpatnext;
    int c, err;
    size_t oldpathc;
    char *bufnext, patbuf[MAXPATHLEN];

    qpatnext = pattern;
    oldpathc = pglob->gl_pathc;
    bufnext = patbuf;

    /* We don't need to check for buffer overflow any more. */
    while ((c = *qpatnext++) != EOS) {
        switch (c) {
        case SEP:
            *bufnext++ = WIN_SEP;
            break;
        case LBRACKET:
            c = *qpatnext;
            if (c == NOT)
                ++qpatnext;
            if (*qpatnext == EOS || strchr((char *) qpatnext + 1, RBRACKET) == NULL) {
                *bufnext++ = LBRACKET;
                if (c == NOT)
                    --qpatnext;
                break;
            }
            *bufnext++ = M_SET;
            if (c == NOT)
                *bufnext++ = M_NOT;
            c = *qpatnext++;
            do {
                *bufnext++ = CHAR(c);
                if (*qpatnext == RANGE && (c = qpatnext[1]) != RBRACKET) {
                    *bufnext++ = M_RNG;
                    *bufnext++ = CHAR(c);
                    qpatnext += 2;
                }
            } while ((c = *qpatnext++) != RBRACKET);
            pglob->gl_flags |= GLOB_MAGCHAR;
            *bufnext++ = M_END;
            break;
        case QUESTION:
            pglob->gl_flags |= GLOB_MAGCHAR;
            *bufnext++ = M_ONE;
            break;
        case STAR:
            pglob->gl_flags |= GLOB_MAGCHAR;
            /* collapse adjacent stars to one,
            * to avoid exponential behavior
            */
            if (bufnext == patbuf || bufnext[-1] != M_ALL)
                *bufnext++ = M_ALL;
            break;
        default:
            *bufnext++ = CHAR(c);
            break;
        }
    }
    *bufnext = EOS;

    if ((err = glob1(patbuf, pglob, limit)) != 0)
        return (err);

    /*
    * If there was no match we are going to append the pattern
    * if GLOB_NOCHECK was specified or if GLOB_NOMAGIC was specified
    * and the pattern did not contain any magic characters
    * GLOB_NOMAGIC is there just for compatibility with csh.
    */
    if (pglob->gl_pathc == oldpathc) {
        if (((pglob->gl_flags & GLOB_NOCHECK) || ((pglob->gl_flags & GLOB_NOMAGIC) && !(pglob->gl_flags & GLOB_MAGCHAR))))
            return (globextend(pattern, pglob, limit));
        else
            return (GLOB_NOMATCH);
    }
    if (!(pglob->gl_flags & GLOB_NOSORT))
        qsort(pglob->gl_pathv + pglob->gl_offs + oldpathc, pglob->gl_pathc - oldpathc, sizeof(char *), compare);
    return (0);
}

static int compare(const void *p, const void *q)
{
    return (strcmp(*(char **) p, *(char **) q));
}

static int glob1(char *pattern, glob_t *pglob, size_t *limit)
{
    char pathbuf[MAXPATHLEN];

    /* A null pathname is invalid -- POSIX 1003.1 sect. 2.4. */
    if (*pattern == EOS)
        return (0);
    return (glob2(pathbuf, pathbuf, pathbuf + MAXPATHLEN - 1, pattern, pglob, limit));
}

/*
* The functions glob2 and glob3 are mutually recursive; there is one level
* of recursion for each segment in the pattern that contains one or more
* meta characters.
*/
static int glob2(char *pathbuf, char *pathend, char *pathend_last, char *pattern, glob_t *pglob, size_t *limit)
{
    struct stat sb;
    char *p, *q;
    int anymeta;

    /*
    * Loop over pattern segments until end of pattern or until
    * segment with meta character found.
    */
    for (anymeta = 0;;) {
        if (*pattern == EOS) {	/* End of pattern? */
            *pathend = EOS;
            if (stat(pathbuf, &sb))
                return (0);

            if (((pglob->gl_flags & GLOB_MARK) && pathend[-1] != SEP && pathend[-1] != WIN_SEP) && (_S_IFDIR & sb.st_mode)) {
                if (pathend + 1 > pathend_last)
                    return (GLOB_ABORTED);
                *pathend++ = WIN_SEP;
                *pathend = EOS;
            }
            ++pglob->gl_matchc;
            return (globextend(pathbuf, pglob, limit));
        }

        /* Find end of next segment, copy tentatively to pathend. */
        q = pathend;
        p = pattern;
        while (*p != EOS && *p != SEP && *p != WIN_SEP) {
            if (ismeta(*p))
                anymeta = 1;
            if (q + 1 > pathend_last)
                return (GLOB_ABORTED);
            *q++ = *p++;
        }

        if (!anymeta) {			/* No expansion, do next segment. */
            pathend = q;
            pattern = p;
            while (*pattern == SEP || *pattern == WIN_SEP) {
                if (pathend + 1 > pathend_last)
                    return (GLOB_ABORTED);
                *pathend++ = *pattern++;
            }
        } else					/* Need expansion, recurse. */
            return (glob3(pathbuf, pathend, pathend_last, pattern, p, pglob, limit));
    }
    /* NOTREACHED */
}

static int glob3(char *pathbuf, char *pathend, char *pathend_last, char *pattern, char *restpattern, glob_t *pglob, size_t *limit)
{
    int err;
    fspr_dir_t *dirp;
    fspr_pool_t *pool;

    fspr_pool_create(&pool, NULL);

    if (pathend > pathend_last)
        return (GLOB_ABORTED);
    *pathend = EOS;
    errno = 0;

    if (fspr_dir_open(&dirp, pathbuf, pool) != APR_SUCCESS) {
        /* TODO: don't call for ENOENT or ENOTDIR? */
        fspr_pool_destroy(pool);
        if (pglob->gl_errfunc) {
            if (pglob->gl_errfunc(pathbuf, errno) || pglob->gl_flags & GLOB_ERR)
                return (GLOB_ABORTED);
        }
        return (0);
    }

    err = 0;

    /* Search directory for matching names. */
    while (dirp) {
        fspr_finfo_t dp;
        unsigned char *sc;
        char *dc;

        if (fspr_dir_read(&dp, APR_FINFO_NAME, dirp) != APR_SUCCESS)
            break;
        if (!(dp.valid & APR_FINFO_NAME) || !(dp.name) || !strlen(dp.name))
            break;

        /* Initial DOT must be matched literally. */
        if (dp.name[0] == DOT && *pattern != DOT)
            continue;
        dc = pathend;
        sc = (unsigned char *) dp.name;

        while (dc < pathend_last && (*dc++ = *sc++) != EOS);

        if (!match(pathend, pattern, restpattern)) {
            *pathend = EOS;
            continue;
        }
        err = glob2(pathbuf, --dc, pathend_last, restpattern, pglob, limit);
        if (err)
            break;
    }

    if (dirp)
        fspr_dir_close(dirp);
    fspr_pool_destroy(pool);
    return (err);
}


/*
* Extend the gl_pathv member of a glob_t structure to accommodate a new item,
* add the new item, and update gl_pathc.
*
* This assumes the BSD realloc, which only copies the block when its size
* crosses a power-of-two boundary; for v7 realloc, this would cause quadratic
* behavior.
*
* Return 0 if new item added, error code if memory couldn't be allocated.
*
* Invariant of the glob_t structure:
*	Either gl_pathc is zero and gl_pathv is NULL; or gl_pathc > 0 and
*	gl_pathv points to (gl_offs + gl_pathc + 1) items.
*/
static int globextend(const char *path, glob_t *pglob, size_t *limit)
{
    char **pathv;
    char *copy;
    size_t i;
    size_t newsize, len;
    const char *p;

    if (*limit && pglob->gl_pathc > *limit) {
        errno = 0;
        return (GLOB_NOSPACE);
    }

    newsize = sizeof(*pathv) * (2 + pglob->gl_pathc + pglob->gl_offs);
    pathv = pglob->gl_pathv ? base_must_realloc((char *) pglob->gl_pathv, newsize) : base_must_malloc(newsize);

    if (pglob->gl_pathv == NULL && pglob->gl_offs > 0) {
        /* first time around -- clear initial gl_offs items */
        pathv += pglob->gl_offs;
        for (i = pglob->gl_offs; i-- > 0;)
            *--pathv = NULL;
    }
    pglob->gl_pathv = pathv;

    for (p = path; *p++;)
        continue;
    len = (size_t) (p - path);
    copy = base_must_malloc(len);
    memcpy(copy, path, len);
    pathv[pglob->gl_offs + pglob->gl_pathc++] = copy;
    pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;
    return (copy == NULL ? GLOB_NOSPACE : 0);
}

/*
* pattern matching function for filenames.  Each occurrence of the *
* pattern causes a recursion level.
*/
static int match(char *name, char *pat, char *patend)
{
    int ok, negate_range;
    char c, k;
    char s1[6];

    while (pat < patend) {
        c = *pat++;
        switch (c & M_MASK) {
        case M_ALL:
            if (pat == patend)
                return (1);
            do
                if (match(name, pat, patend))
                    return (1);
            while (*name++ != EOS);
            return (0);
        case M_ONE:
            if (*name++ == EOS)
                return (0);
            break;
        case M_SET:
            ok = 0;
            if ((k = *name++) == EOS)
                return (0);
            if ((negate_range = ((*pat & M_MASK) == M_NOT)) != EOS)
                ++pat;
            while (((c = *pat++) & M_MASK) != M_END)
                if ((*pat & M_MASK) == M_RNG) {
                    memset(s1, 0, sizeof(s1));
                    s1[0] = c;
                    s1[2] = k;
                    s1[4] = pat[1];
                    if (strcoll(&s1[0], &s1[2]) <= 0 && strcoll(&s1[2], &s1[4]) <= 0)
                        ok = 1;
                    pat += 2;
                } else if (c == k)
                    ok = 1;
                if (ok == negate_range)
                    return (0);
                break;
        default:
            if (*name++ != c)
                return (0);
            break;
        }
    }
    return (*name == EOS);
}

/* Free allocated data belonging to a glob_t structure. */
void globfree(glob_t *pglob)
{
    size_t i;
    char **pp;

    if (pglob->gl_pathv != NULL) {
        pp = pglob->gl_pathv + pglob->gl_offs;
        for (i = pglob->gl_pathc; i--; ++pp)
            if (*pp)
                free(*pp);
        free(pglob->gl_pathv);
        pglob->gl_pathv = NULL;
    }
}

#pragma warning(pop)
#endif

/* For Emacs:
* Local Variables:
* mode:c
* indent-tabs-mode:t
* tab-width:4
* c-basic-offset:4
* End:
* For VIM:
* vim:set softtabstop=4 shiftwidth=4 tabstop=4 noet:
*/
