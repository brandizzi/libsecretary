#!/bin/sh
find  src/test/ | xargs grep 'CuTest \*' | wc -l
find  src/test/ | xargs grep 'SUITE_ADD_TEST' | wc -l
find  src/test/ | xargs sed -n 's/.*void *\(test_.*\)(.*/\1/p' > exists
find  src/test/ | xargs sed -n '/SUITE_ADD_TEST(suite, *$/{n;s/\n */ /;};s/.*SUITE_ADD_TEST(suite, \(.*\)).*/\1/p'  > runs
sort exists > sorted_exists
sort runs > sorted_runs
diff sorted_exists sorted_runs
rm exists sorted_exists runs sorted_runs
