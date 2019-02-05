instances=$(ls input/*.txt)

for instance in $instances;
do
    for exec_time in 10 30 60;
    do
        echo "Running ECVRPTW on $instance for $exec_time s :"
        cmake-build-debug/ECVRPTW $instance $exec_time
    done
done

echo "Done."