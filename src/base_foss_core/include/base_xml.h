/*
* FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
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
* The Original Code is FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
*
* The Initial Developer of the Original Code is
* Anthony Minessale II <anthm@freebase.org>
* Portions created by the Initial Developer are Copyright (C)
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*
* Anthony Minessale II <anthm@freebase.org>
*
*
* base_xml.h -- XML PARSER
*
* Derived from EZXML http://ezxml.sourceforge.net
* Original Copyright
*
* Copyright 2004, 2005 Aaron Voisine <aaron@voisine.org>
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

#ifndef BASE_XML_H
#define BASE_XML_H
#include <base.h>

/* Use UTF-8 as the general encoding */
#define USE_UTF_8_ENCODING BASE_TRUE

struct base_xml_binding;

///\defgroup xml1 XML Library Functions
///\ingroup core1
///\{
BASE_BEGIN_EXTERN_C
#define BASE_XML_BUFSIZE 1024	// size of internal memory buffers
typedef enum {
    BASE_XML_ROOT = (1 << 0),	// root
    BASE_XML_NAMEM = (1 << 1),	// name is malloced
    BASE_XML_TXTM = (1 << 2),	// txt is malloced
    BASE_XML_DUP = (1 << 3),	// attribute name and value are strduped
    BASE_XML_CDATA = (1 << 4) // body is in CDATA
} base_xml_flag_t;

/*! \brief A representation of an XML tree */
struct base_xml {
    /*! tag name */
    char *name;
    /*! tag attributes { name, value, name, value, ... NULL } */
    char **attr;
    /*! tag character content, empty string if none */
    char *txt;
    /*! path to free on destroy */
    char *free_path;
    /*! tag offset from start of parent tag character content */
    base_size_t off;
    /*! next tag with same name in this section at this depth */
    base_xml_t next;
    /*! next tag with different name in same section and depth */
    base_xml_t sibling;
    /*! next tag, same section and depth, in original order */
    base_xml_t ordered;
    /*! head of sub tag list, NULL if none */
    base_xml_t child;
    /*! parent tag, NULL if current tag is root tag */
    base_xml_t parent;
    /*! flags */
    uint32_t flags;
    /*! is_base_xml_root bool */
    base_bool_t is_base_xml_root_t;
    uint32_t refs;
    /*! pointer to end of opening tag, '>', in the original parsed text */
    const char *open;
    /*! pointer to start of closing tag, '<', in the original parsed text */
    const char *close;
};

/*!
* \brief Parses a string into a base_xml_t, ensuring the memory will be freed with base_xml_free
* \param s The string to parse
* \param dup true if you want the string to be strdup()'d automatically
* \return the base_xml_t or NULL if an error occured
*/
BASE_DECLARE(base_xml_t) base_xml_parse_str_dynamic(_In_z_ char *s, _In_ base_bool_t dup);

/*!
* \brief Parses a string into a base_xml_t
* \param s The string to parse
* \return the base_xml_t or NULL if an error occured
*/
#define base_xml_parse_str_dup(x)  base_xml_parse_str_dynamic(x, BASE_TRUE)

///\brief Given a string of xml data and its length, parses it and creates an base_xml
///\ structure. For efficiency, modifies the data by adding null terminators
///\ and decoding ampersand sequences. If you don't want this, copy the data and
///\ pass in the copy. Returns NULL on failure.
///\param s a string
///\param len the length of the string
///\return a formated xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_parse_str(_In_z_ char *s, _In_ base_size_t len);

///\brief A wrapper for base_xml_parse_str() that accepts a file descriptor. First
///\ attempts to mem map the file. Failing that, reads the file into memory.
///\ Returns NULL on failure.
///\param fd
///\return a formated xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_parse_fd(int fd);

///\brief a wrapper for base_xml_parse_fd() that accepts a file name
///\param file a file to parse
///\return a formated xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_parse_file(_In_z_ const char *file);

BASE_DECLARE(base_xml_t) base_xml_parse_file_simple(_In_z_ const char *file);

///\brief Wrapper for base_xml_parse_str() that accepts a file stream. Reads the entire
///\ stream into memory and then parses it. For xml files, use base_xml_parse_file()
///\ or base_xml_parse_fd()
///\param fp a FILE pointer to parse
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_parse_fp(_In_ FILE * fp);

///\brief returns the first child tag (one level deeper) with the given name or NULL
///\ if not found
///\param xml an xml node
///\param name the name of the child tag
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_child(_In_ base_xml_t xml, _In_z_ const char *name);

///\brief find a child tag in a node called 'childname' with an attribute 'attrname' which equals 'value'
///\param node the xml node
///\param childname the child tag name
///\param attrname the attribute name
///\param value the value
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_find_child(_In_ base_xml_t node, _In_z_ const char *childname, _In_opt_z_ const char *attrname,
    _In_opt_z_ const char *value);
BASE_DECLARE(base_xml_t) base_xml_find_child_multi(_In_ base_xml_t node, _In_z_ const char *childname, ...);

///\brief returns the next tag of the same name in the same section and depth or NULL
///\ if not found
///\param xml an xml node
///\return an xml node or NULL
#define base_xml_next(xml) ((xml) ? xml->next : NULL)

///\brief Returns the Nth tag with the same name in the same section at the same depth
///\ or NULL if not found. An index of 0 returns the tag given.
///\param xml the xml node
///\param idx the index
///\return an xml node or NULL
base_xml_t base_xml_idx(_In_ base_xml_t xml, _In_ int idx);

///\brief returns the name of the given tag
///\param xml the xml node
///\return the name
#define base_xml_name(xml) ((xml) ? xml->name : NULL)

///\brief returns the given tag's character content or empty string if none
///\param xml the xml node
///\return the content
#define base_xml_txt(xml) ((xml) ? xml->txt : "")

///\brief returns the value of the requested tag attribute, or NULL if not found
///\param xml the xml node
///\param attr the attribute
///\return the value
BASE_DECLARE(const char *) base_xml_attr(_In_opt_ base_xml_t xml, _In_opt_z_ const char *attr);

///\brief returns the value of the requested tag attribute, or "" if not found
///\param xml the xml node
///\param attr the attribute
///\return the value
BASE_DECLARE(const char *) base_xml_attr_soft(_In_ base_xml_t xml, _In_z_ const char *attr);

///\brief Traverses the base_xml structure to retrieve a specific subtag. Takes a
///\ variable length list of tag names and indexes. The argument list must be
///\ terminated by either an index of -1 or an empty string tag name. Example:
///\ title = base_xml_get(library, "shelf", 0, "book", 2, "title", -1);
///\ This retrieves the title of the 3rd book on the 1st shelf of library.
///\ Returns NULL if not found.
///\param xml the xml node
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_get(_In_ base_xml_t xml,...);

///\brief Converts an base_xml structure back to xml in html format. Returns a string of html data that
///\ must be freed.
///\param xml the xml node
///\param prn_header add <?xml version..> header too
///\param use_utf8_encoding encoding into ampersand entities for UTF-8 chars
///\return the ampersanded html text string to display xml
#define base_xml_toxml(xml, prn_header) base_xml_toxml_ex(xml, prn_header, USE_UTF_8_ENCODING)
#define base_xml_toxml_nolock(xml, prn_header) base_xml_toxml_nolock_ex(xml, prn_header, USE_UTF_8_ENCODING)
#define base_xml_tohtml(xml, prn_header) base_xml_tohtml_ex(xml, prn_header, USE_UTF_8_ENCODING)

BASE_DECLARE(char *) base_xml_toxml_ex(_In_ base_xml_t xml, _In_ base_bool_t prn_header, base_bool_t use_utf8_encoding);
BASE_DECLARE(char *) base_xml_toxml_nolock_ex(base_xml_t xml, _In_ base_bool_t prn_header, base_bool_t use_utf8_encoding);
BASE_DECLARE(char *) base_xml_tohtml_ex(_In_ base_xml_t xml, _In_ base_bool_t prn_header, base_bool_t use_utf8_encoding);

///\brief Converts an base_xml structure back to xml using the buffer passed in the parameters.
///\param xml the xml node
///\param buf buffer to use
///\param buflen size of buffer
///\param offset offset to start at
///\param prn_header add <?xml version..> header too
///\param use_utf8_encoding encoding into ampersand entities for UTF-8 chars
///\return the xml text string
#define base_xml_toxml_buf(xml, buf, buflen, offset, prn_header) base_xml_toxml_buf_ex(xml, buf, buflen, offset, prn_header, USE_UTF_8_ENCODING);
BASE_DECLARE(char *) base_xml_toxml_buf_ex(_In_ base_xml_t xml, _In_z_ char *buf, _In_ base_size_t buflen, _In_ base_size_t offset,
    _In_ base_bool_t prn_header, base_bool_t use_utf8_encoding);


///\brief returns a NULL terminated array of processing instructions for the given
///\ target
///\param xml the xml node
///\param target the instructions
///\return the array
BASE_DECLARE(const char **) base_xml_pi(_In_ base_xml_t xml, _In_z_ const char *target);

///\brief frees the memory allocated for an base_xml structure
///\param xml the xml node
///\note in the case of the root node the readlock will be lifted
BASE_DECLARE(void) base_xml_free(_In_opt_ base_xml_t xml);
BASE_DECLARE(void) base_xml_free_in_thread(_In_ base_xml_t xml, _In_ int stacksize);

///\brief returns parser error message or empty string if none
///\param xml the xml node
///\return the error string or nothing
BASE_DECLARE(const char *) base_xml_error(_In_ base_xml_t xml);

///\brief returns a new empty base_xml structure with the given root tag name
///\param name the name of the new root tag
BASE_DECLARE(base_xml_t) base_xml_new(_In_opt_z_ const char *name);

///\brief wrapper for base_xml_new() that strdup()s name
///\param name the name of the root
///\return an xml node or NULL
#define base_xml_new_d(name) base_xml_set_flag(base_xml_new(strdup(name)), BASE_XML_NAMEM)

///\brief Adds a child tag. off is the offset of the child tag relative to the start
///\ of the parent tag's character content. Returns the child tag.
///\param xml the xml node
///\param name the name of the tag
///\param off the offset
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_add_child(_In_ base_xml_t xml, _In_z_ const char *name, _In_ base_size_t off);

///\brief wrapper for base_xml_add_child() that strdup()s name
///\param xml the xml node
///\param name the name of the child
///\param off the offset
BASE_DECLARE(base_xml_t) base_xml_add_child_d(_In_ base_xml_t xml, _In_z_ const char *name, _In_ base_size_t off);

///\brief sets the character content for the given tag and returns the tag
///\param xml the xml node
///\param txt the text
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_set_txt(base_xml_t xml, const char *txt);

///\brief wrapper for base_xml_set_txt() that strdup()s txt
///\ sets the character content for the given tag and returns the tag
///\param xml the xml node
///\param txt the text
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_set_txt_d(base_xml_t xml, const char *txt);

///\brief Sets the given tag attribute or adds a new attribute if not found. A value
///\ of NULL will remove the specified attribute.
///\param xml the xml node
///\param name the attribute name
///\param value the attribute value
///\return the tag given
BASE_DECLARE(base_xml_t) base_xml_set_attr(base_xml_t xml, const char *name, const char *value);

///\brief Wrapper for base_xml_set_attr() that strdup()s name/value. Value cannot be NULL
///\param xml the xml node
///\param name the attribute name
///\param value the attribute value
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_set_attr_d(base_xml_t xml, const char *name, const char *value);

///\brief Wrapper for base_xml_set_attr() that strdup()s name/value. Value cannot be NULL
///\param xml the xml node
///\param name the attribute name
///\param value the attribute value
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_set_attr_d_buf(base_xml_t xml, const char *name, const char *value);

///\brief sets a flag for the given tag and returns the tag
///\param xml the xml node
///\param flag the flag to set
///\return an xml node or NULL
BASE_DECLARE(base_xml_t) base_xml_set_flag(base_xml_t xml, base_xml_flag_t flag);

///\brief removes a tag along with its subtags without freeing its memory
///\param xml the xml node
BASE_DECLARE(base_xml_t) base_xml_cut(_In_ base_xml_t xml);

///\brief inserts an existing tag into an ezxml structure
BASE_DECLARE(base_xml_t) base_xml_insert(_In_ base_xml_t xml, _In_ base_xml_t dest, _In_ base_size_t off);

///\brief Moves an existing tag to become a subtag of dest at the given offset from
///\ the start of dest's character content. Returns the moved tag.
#define base_xml_move(xml, dest, off) base_xml_insert(base_xml_cut(xml), dest, off)

///\brief removes a tag along with all its subtags
#define base_xml_remove(xml) base_xml_free(base_xml_cut(xml))

///\brief set new core xml root
BASE_DECLARE(base_status_t) base_xml_set_root(base_xml_t new_main);

///\brief Set and alternate function for opening xml root
BASE_DECLARE(base_status_t) base_xml_set_open_root_function(base_xml_open_root_function_t func, void *user_data);

///\brief open the Core xml root
///\param reload if it's is already open close it and open it again as soon as permissable (blocking)
///\param err a pointer to set error strings
///\return the xml root node or NULL
BASE_DECLARE(base_xml_t) base_xml_open_root(_In_ uint8_t reload, _Out_ const char **err);

///\brief initilize the core XML backend
///\param pool a memory pool to use
///\param err a pointer to set error strings
///\return BASE_STATUS_SUCCESS if successful
BASE_DECLARE(base_status_t) base_xml_init(_In_ base_memory_pool_t *pool, _Out_ const char **err);

BASE_DECLARE(base_status_t) base_xml_reload(const char **err);

BASE_DECLARE(base_status_t) base_xml_destroy(void);

///\brief retrieve the core XML root node
///\return the xml root node
///\note this will cause a readlock on the root until it's released with \see base_xml_free
BASE_DECLARE(base_xml_t) base_xml_root(void);

///\brief locate an xml pointer in the core registry
///\param section the section to look in
///\param tag_name the type of tag in that section
///\param key_name the name of the key
///\param key_value the value of the key
///\param root a pointer to point at the root node
///\param node a pointer to the requested node
///\param params optional URL formatted params to pass to external gateways
///\return BASE_STATUS_SUCCESS if successful root and node will be assigned
BASE_DECLARE(base_status_t) base_xml_locate(_In_z_ const char *section,
    _In_opt_z_ const char *tag_name,
    _In_opt_z_ const char *key_name,
    _In_opt_z_ const char *key_value,
    _Out_ base_xml_t *root,
    _Out_ base_xml_t *node, _In_opt_ base_event_t *params, _In_ base_bool_t clone);

BASE_DECLARE(base_status_t) base_xml_locate_domain(_In_z_ const char *domain_name, _In_opt_ base_event_t *params, _Out_ base_xml_t *root,
    _Out_ base_xml_t *domain);

BASE_DECLARE(base_status_t) base_xml_locate_group(_In_z_ const char *group_name,
    _In_z_ const char *domain_name,
    _Out_ base_xml_t *root,
    _Out_ base_xml_t *domain, _Out_ base_xml_t *group, _In_opt_ base_event_t *params);

BASE_DECLARE(base_status_t) base_xml_locate_user(_In_z_ const char *key,
    _In_z_ const char *user_name,
    _In_z_ const char *domain_name,
    _In_opt_z_ const char *ip,
    _Out_ base_xml_t *root, _Out_ base_xml_t *domain, _Out_ base_xml_t *user,
    _Out_opt_ base_xml_t *ingroup, _In_opt_ base_event_t *params);

BASE_DECLARE(base_status_t) base_xml_locate_user_in_domain(_In_z_ const char *user_name, _In_ base_xml_t domain, _Out_ base_xml_t *user,
    _Out_opt_ base_xml_t *ingroup);


BASE_DECLARE(base_status_t) base_xml_locate_user_merged(const char *key, const char *user_name, const char *domain_name,
    const char *ip, base_xml_t *user, base_event_t *params);
BASE_DECLARE(uint32_t) base_xml_clear_user_cache(const char *key, const char *user_name, const char *domain_name);
BASE_DECLARE(void) base_xml_merge_user(base_xml_t user, base_xml_t domain, base_xml_t group);

BASE_DECLARE(base_xml_t) base_xml_dup(base_xml_t xml);

///\brief open a config in the core registry
///\param file_path the name of the config section e.g. modules.conf
///\param node a pointer to point to the node if it is found
///\param params optional URL formatted params to pass to external gateways
///\return the root xml node associated with the current request or NULL
BASE_DECLARE(base_xml_t) base_xml_open_cfg(_In_z_ const char *file_path, _Out_ base_xml_t *node, _In_opt_ base_event_t *params);

///\brief bind a search function to an external gateway
///\param function the search function to bind
///\param sections a bitmask of sections you wil service
///\param user_data a pointer to private data to be used during the callback
///\return BASE_STATUS_SUCCESS if successful
///\note gateway functions will be executed in the order they were binded until a success is found else the root registry will be used

BASE_DECLARE(void) base_xml_set_binding_sections(_In_ base_xml_binding_t *binding, _In_ base_xml_section_t sections);
BASE_DECLARE(void) base_xml_set_binding_user_data(_In_ base_xml_binding_t *binding, _In_opt_ void *user_data);
BASE_DECLARE(base_xml_section_t) base_xml_get_binding_sections(_In_ base_xml_binding_t *binding);
BASE_DECLARE(void *) base_xml_get_binding_user_data(_In_ base_xml_binding_t *binding);

BASE_DECLARE(base_status_t) base_xml_bind_search_function_ret(_In_ base_xml_search_function_t function, _In_ base_xml_section_t sections,
    _In_opt_ void *user_data, base_xml_binding_t **ret_binding);
#define base_xml_bind_search_function(_f, _s, _u) base_xml_bind_search_function_ret(_f, _s, _u, NULL)


BASE_DECLARE(base_status_t) base_xml_unbind_search_function(_In_ base_xml_binding_t **binding);
BASE_DECLARE(base_status_t) base_xml_unbind_search_function_ptr(_In_ base_xml_search_function_t function);

///\brief parse a string for a list of sections
///\param str a | delimited list of section names
///\return the section mask
BASE_DECLARE(base_xml_section_t) base_xml_parse_section_string(_In_opt_z_ const char *str);

BASE_DECLARE(int) base_xml_std_datetime_check(base_xml_t xcond, int *offset, const char *tzname);

BASE_DECLARE(base_status_t) base_xml_locate_language(base_xml_t *root, base_xml_t *node, base_event_t *params, base_xml_t *language, base_xml_t *phrases, base_xml_t *macros, const char *str_language);

BASE_END_EXTERN_C
///\}
#endif // _BASE_XML_H
