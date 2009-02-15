#include <glib.h>
#include <twitter-glib/twitter-glib.h>

#ifndef __TWITTER_TEST_MAIN_H__
#define __TWITTER_TEST_MAIN_H__

#define twitter_test_add(unit_name,func)                G_STMT_START {  \
        extern void func (void);                                        \
        g_test_add_func ("/twitter" unit_name, func);   } G_STMT_END

#define twitter_test_skip_if_fail(cond,unit_name,func)  G_STMT_START {  \
        if ((cond)) {                                                   \
                g_test_add_func ("/twitter/skipped" unit_name,          \
                                 twitter_test_skip);                    \
        } else {                                                        \
                twitter_test_add (unit_name, func);                     \
        }                                               } G_STMT_END

void twitter_test_skip (void);

#endif /* __TWITTER_TEST_MAIN_H__ */
