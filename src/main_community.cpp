// File: main_community.cpp
// -- community detection, sample main file
//-----------------------------------------------------------------------------
// Community detection
// Based on the article "Fast unfolding of community hierarchies in large networks"
// Copyright (C) 2008 V. Blondel, J.-L. Guillaume, R. Lambiotte, E. Lefebvre
//
// This program must not be distributed without agreement of the above mentionned authors.
//-----------------------------------------------------------------------------
// Author   : E. Lefebvre, adapted by J.-L. Guillaume and then Anthony Perez and Nicolas Dugué for directed modularity
//-----------------------------------------------------------------------------
// see readme.txt for more details

#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "../include/graph_binary.hpp"
#include "../include/community.h"

using namespace std;

string filename = "";
string filename_w = "";
string filename_part = "";
int type = UNWEIGHTED;
int nb_pass = 0;
double precision = 0.000001;
int display_level = -2;
int k1 = 16;
bool verbose = false;

void
usage(char * prog_name,
    const char * more) {
    cerr << more;
    cerr << "usage: " << prog_name << " input_file [-w weight_file] [-p part_file] [-q epsilon] [-l display_level] [-v] [-h]" << endl << endl;
    cerr << "input_file: file containing the graph to decompose in communities." << endl;
    cerr << "-w file\tread the graph as a weighted one (weights are set to 1 otherwise)." << endl;
    cerr << "-p file\tstart the computation with a given partition instead of the trivial partition." << endl;
    cerr << "\tfile must contain lines \"node community\"." << endl;
    cerr << "-q eps\ta given pass stops when the modularity is increased by less than epsilon." << endl;
    cerr << "-l k\tdisplays the graph of level k rather than the hierachical structure." << endl;
    cerr << "\tif k=-1 then displays the hierarchical structure rather than the graph at a given level." << endl;
    cerr << "-v\tverbose mode: gives computation time, information about the hierarchy and modularity." << endl;
    cerr << "-h\tshow this usage message." << endl;
    exit(0);
}

void
parse_args(int argc, char ** argv) {
    if (argc < 2)
        usage(argv[0], "Bad arguments number\n");

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'w':
                type = WEIGHTED;
                filename_w = argv[i + 1];
                i++;
                break;
            case 'p':
                filename_part = argv[i + 1];
                i++;
                break;
            case 'q':
                precision = atof(argv[i + 1]);
                i++;
                break;
            case 'l':
                display_level = atoi(argv[i + 1]);
                i++;
                break;
            case 'k':
                k1 = atoi(argv[i + 1]);
                i++;
                break;
            case 'v':
                verbose = true;
                break;
            default:
                usage(argv[0], "Unknown option\n");
            }
        } else {
            if (filename == "")
                filename = argv[i];
            else
                usage(argv[0], "More than one filename\n");
        }
    }
}

void
display_time(const char * str) {
    time_t rawtime;
    time( & rawtime);
    cerr << str << ": " << ctime( & rawtime);
}

int
main(int argc, char ** argv) {
    srand(time(NULL));

    ofstream foutput;
    foutput.open("modularity_values_louvain_oriente.txt", fstream::app | fstream::binary);

    parse_args(argc, argv);
    time_t time_begin, time_end;
    time( & time_begin);
    if (verbose)
        display_time("Begin");
    Community *c = new Community(filename, type, -1, precision);
    //Community *c = new Community(filename, "", type, -1, precision);
    cerr << "OK" << endl;
    if (filename_part != "")
        c->init_partition(filename_part);
    Graph * g = NULL;
    bool improvement = true;
    double mod = c->modularity(), new_mod;
    int nb_pass = 0;
    int level = 0;

    do {
        nb_pass++;
        /* TODO: does not work with (*g).nb_nodes if g is NULL */
        if (verbose) {
            cerr << "level " << level << ":\n";
            display_time("  start computation");
            cerr << "  network size: " <<
                ( * (c->g)).nb_nodes << " nodes, " <<
                ( * (c->g)).nb_links_out << " links, " <<
                ( * (c->g)).total_weight << " weight." << endl;
        }

        improvement = c->one_level();
        new_mod = c->modularity();
        if (++level == display_level)
            (* (c->g)).display();
        if (display_level == -1)
            c->display_partition();
        g = c->partition2graph_binary();
        delete c;
        c = new Community(g, -1, precision);
        delete g;
        /* FIXME: memory leak for not destroying objects */
        if (verbose)
            cerr << "  modularity increased from " << mod << " to " << new_mod << endl;

        mod = new_mod;
        if (verbose)
            display_time("  end computation");
        if (filename_part != "" && level == 1) // do at least one more computation if partition is provided
            improvement = true;
    } while (improvement);

    delete c;
    time( & time_end);
    if (verbose) {
        display_time("End");
        cerr << "Total duration: " << (time_end - time_begin) << " sec." << endl;
    }
    cerr << new_mod << endl;
    foutput << new_mod << endl;
    foutput.close();
}
