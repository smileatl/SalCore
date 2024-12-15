#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

BASE_BEGIN_EXTERN_C

/**
 * Module structures.  Just about everything is dispatched through
 * these, directly or indirectly (through the command and handler
 * tables).
 */
//typedef struct module_struct module;

//struct module_struct {
//    /** API version, *not* module version; check that module is
//     * compatible with this version of the server.
//     */
//    int version;
//    /** API minor version. Provides API feature milestones. Not checked
//     *  during module init */
//    int minor_version;
//    /** Index to this modules structures in config vectors.  */
//    int module_index;
//
//    /** The name of the module's C file */
//    const char *name;
//    /** The handle for the DSO.  Internal use only */
//    void *dynamic_load_handle;
//
//    /** A pointer to the next module in the list
//     *  @var module_struct *next
//     */
//    struct module_struct *next;
//
//    /** Magic Cookie to identify a module structure;  It's mainly
//     *  important for the DSO facility (see also mod_so).  */
//    unsigned long magic;
//
//    /** Function to allow MPMs to re-write command line arguments.  This
//     *  hook is only available to MPMs.
//     *  @param The process that the server is running in.
//     */
//    void (*rewrite_args) (process_rec *process);
//    /** Function to allow all modules to create per directory configuration
//     *  structures.
//     *  @param p The pool to use for all allocations.
//     *  @param dir The directory currently being processed.
//     *  @return The per-directory structure created
//     */
//    void *(*create_dir_config) (apr_pool_t *p, char *dir);
//    /** Function to allow all modules to merge the per directory configuration
//     *  structures for two directories.
//     *  @param p The pool to use for all allocations.
//     *  @param base_conf The directory structure created for the parent directory.
//     *  @param new_conf The directory structure currently being processed.
//     *  @return The new per-directory structure created
//     */
//    void *(*merge_dir_config) (apr_pool_t *p, void *base_conf, void *new_conf);
//    /** Function to allow all modules to create per server configuration
//     *  structures.
//     *  @param p The pool to use for all allocations.
//     *  @param s The server currently being processed.
//     *  @return The per-server structure created
//     */
//    void *(*create_server_config) (apr_pool_t *p, server_rec *s);
//    /** Function to allow all modules to merge the per server configuration
//     *  structures for two servers.
//     *  @param p The pool to use for all allocations.
//     *  @param base_conf The directory structure created for the parent directory.
//     *  @param new_conf The directory structure currently being processed.
//     *  @return The new per-directory structure created
//     */
//    void *(*merge_server_config) (apr_pool_t *p, void *base_conf,
//                                  void *new_conf);
//
//    /** A command_rec table that describes all of the directives this module
//     * defines. */
//    const command_rec *cmds;
//
//    /** A hook to allow modules to hook other points in the request processing.
//     *  In this function, modules should call the ap_hook_*() functions to
//     *  register an interest in a specific step in processing the current
//     *  request.
//     *  @param p the pool to use for all allocations
//     */
//    void (*register_hooks) (apr_pool_t *p);
//
//    /** A bitmask of AP_MODULE_FLAG_* */
//    int flags;
//};

BASE_END_EXTERN_C

#endif // BASE_CONFIG_H