/**
 * Fork/join parallelism in Java
 *
 * Figure 4.18
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts  - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

import java.util.concurrent.*;
import java.util.*;

public class Sorter extends RecursiveAction
{
    static final int SIZE = 10000;
    static final int THRESHOLD = 10;

    private int begin;
    private int end;
    private int[] array;

    public Sorter(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
        System.out.println("Task: " + begin + ":" + end);
    }

    protected void compute() {
        if (end < begin) { return; } 
        if (end - begin < THRESHOLD) {
            Arrays.sort(array, begin, end + 1);
        }
        else {
            // divide stage 
            int partitionIndex = partition();
           
            System.out.println("Partitioning: " + begin + ":" + partitionIndex + ":" + end);
                    
            Sorter leftTask = new Sorter(begin, partitionIndex - 1, array);
            Sorter rightTask = new Sorter(partitionIndex, end, array);

            invokeAll(leftTask, rightTask);
        }
    }

    private int partition() {
        int pivot = array[end];
        int i = begin - 1;

        for(int j = begin; j < end; j++) {
            if (array[j] <= pivot) {
                i++;
                swap(i, j);
            }
        }

        i++;
        swap(i, end);

        return i;
    }

    void swap(int i, int j) {
        int t = array[i];
        array[i] = array[j];
        array[j] = t;
    }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];

		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(1000);
		}		
	
        OutputSorting(array);

		// use fork-join parallelism to sort the array
		Sorter task = new Sorter(0, SIZE-1, array);

		pool.invoke(task);

        OutputSorting(array);
	}

    static void OutputSorting(int[] array){
        int fails = 0;

		for (int i = 0; i < SIZE; i++) {
		    System.out.print(", " + array[i]);
		    if (i > 0 && array[i] < array[i-1]) {
                System.out.println();
                System.out.println(".. not sorted at " + i);

                fails++;

                    break;
            }
		}		
        System.out.println("");
    
    }
}

