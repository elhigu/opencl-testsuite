// TODO: provide echo script or change runlines to be platform independent...
//       maybe sh is actually required in windows too...

// RUN: %{python} -c "print '''Hello worlds this documents some subtitutions which are available in test scripts'''"
// RUN: %{python} -c "print '''Substitute %\%: %%'''"
// RUN: %{python} -c "print '''Substitute %\s: %s'''"
// RUN: %{python} -c "print '''Substitute %\S: %S'''"
// RUN: %{python} -c "print '''Substitute %\p: %p'''"
// RUN: %{python} -c "print '''Substitute %\{pathsep}: %{pathsep}'''"
// RUN: %{python} -c "print '''Substitute %\t: %t'''"
// RUN: %{python} -c "print '''Substitute %\T: %T'''"
// RUN: %{python} -c "print '''Substitute %\{src_root}: %{src_root}'''"
// RUN: %{python} -c "print '''Substitute %\{inputs}: %{inputs}'''"
// RUN: %{python} -c "print '''Substitute %\{lit}: %{lit}'''"
// RUN: %{python} -c "print '''Substitute %\{python}: %{python}'''"
// RUN: %{python} -c "print '''Substitute %\{device}: <This one is not shown now because for some reason it crashes shell parser>'''"
// RUN: %{python} -c "print '''Substitute %\{device_id}: %{device_id}'''"
// RUN: %{python} -c "print '''Substitute %\{ocl_tester}: %{ocl_tester}'''"
