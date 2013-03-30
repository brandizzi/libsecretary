#!/bin/sh
find  test/ | xargs grep 'CuTest \*' | wc -l
find  test/ | xargs grep 'SUITE_ADD_TEST' | wc -l
find  test/ | xargs sed -n 's/.*void *\(test_.*\)(.*/\1/p' > exists
find  test/ | xargs sed -n '/SUITE_ADD_TEST(suite, *$/{n;s/suite, *test/suite, test/;};s/.*SUITE_ADD_TEST(suite, \(.*\)).*/\1/p'  > runs
sort exists > sorted_exists
sort runs > sorted_runs
diff sorted_exists sorted_runs
rm exists sorted_exists runs sorted_runs
