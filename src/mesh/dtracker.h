// sparselizard - Copyright (C) see copyright file.
//
// See the LICENSE file for license information. Please report all
// bugs and problems to <alexandre.halbach at gmail.com>.


// This object manages the decomposition of the mesh into domains.

#ifndef DTRACKER_H
#define DTRACKER_H

#include <iostream>
#include <vector>
#include "rawmesh.h"
#include "slmpi.h"


class dtracker
{

    private:
    
        // Number of overlap layers (0 for no-overlap):
        int mynumoverlaplayers = -1;

        std::weak_ptr<rawmesh> myrawmesh;

        // Neighbours (without this rank and without duplicates, sorted ascendingly):
        std::vector<int> myneighbours = {};
        // Direct access (length is numranks):
        std::vector<bool> myisneighbour = {};
        // No-overlap interfaces (length is 3*numranks, -1 if none).
        // Entry 3*r+i is the interface of i-dimensional elements with rank r:
        std::vector<int> mynooverlapinterfaces = {};
        
        // Map entry [n][type][i] gives the element number of type 'type' in this domain that corresponds to the
        // ith element of that type in the nth neighbour domain. Only elements in the outer-overlap/no-overlap
        // interfaces can be mapped. Curvature nodes cannot be mapped. The map has value -1 by default.
        std::vector<std::vector<std::vector<int>>> mymaptothisdomain = {};
        
        // Global node numbers for every domain node (unrelated to the local node numbers, -1 for curvature nodes):
        std::vector<long long int> myglobalnodenumbers = {};
        

        // Discover up to 'numtrialelements' neighbours that share cell-1 dimension elements with this rank.
        // The barycenter of all elements shared with the neighbours to discover must be provided as argument.
        // The number of barycenters provided on all ranks is returned (if all zero an empty vector is returned).
        std::vector<int> discoversomeneighbours(int numtrialelements, std::vector<double>& interfaceelembarys, std::vector<int>& neighboursfound);

        // Upon return 'inneighbours[i]' is the number of the neighbour touching the ith interface element (-1 if no neighbour touching).
        // The neighbours provided must be unique. The number of interface elements for each rank must be provided in 'allnumelementsininterface'.
        void discoverinterfaces(std::vector<int> neighbours, std::vector<double>& interfaceelembarys, std::vector<int>& allnumelementsininterface, std::vector<int>& inneighbour);

        // Find new interfaces and populate the output accordingly. Return false if no more interfaces can be found on any rank.
        bool discovercrossinterfaces(std::vector<int>& interfacenodelist, std::vector<int>& interfaceedgelist, std::vector<std::vector<bool>>& isnodeinneighbours, std::vector<std::vector<bool>>& isedgeinneighbours);

        // Map the outer-overlap/no-overlap interfaces:
        void mapnooverlapinterfaces(void);
        void mapoverlapinterfaces(void);
        
        // Create the global node numbers for every domain node:
        void createglobalnodenumbersnooverlap(void);
        void createglobalnodenumbersoverlap(void);

    public:

        dtracker(std::shared_ptr<rawmesh> rm);

        std::shared_ptr<rawmesh> getrawmesh(void);
        
        bool isoverlap(void);
        
        // Set manually the no-overlap connectivity of this rank.
        // 'nooverlapinterfaces[3*i+j]' is the interface of j-dimensional elements with the ith neighbour (-1 if none).
        void setconnectivity(std::vector<int>& neighbours, std::vector<int>& nooverlapinterfaces);
        // Discover automatically the no-overlap connectivity of this rank.
        // This works for any geometry whose global skin region does not intersect itself.
        // Region 'nooverlapinterface' must include all elements of dimension cell-1 which touch the neighbour domains.
        void discoverconnectivity(int nooverlapinterface, int numtrialelements = 10, int verbosity = 0);

        // Map the outer-overlap/no-overlap interfaces:
        void mapinterfaces(void);
        
        // Create the global node numbers for every domain node:
        void createglobalnodenumbers(void);
        
        int countneighbours(void);
        std::vector<int> getneighbours(void);
        int getneighbour(int neighbourindex);

        bool isneighbour(int neighbour);
        // Return -1 if not defined:
        int getnooverlapinterface(int neighbour, int elementdimension);

        std::vector<std::vector<std::vector<int>>>* getmap(void);
        
        long long int* getglobalnodenumbers(void);
        void writeglobalnodenumbers(std::string filename);

        // Print connectivity information:
        void print(void);
        // Write no-overlap domain interfaces:
        void writeinterfaces(std::string filename);

};

#endif

