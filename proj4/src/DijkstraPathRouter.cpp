#include "DijkstraPathRouter.h"
#include<vector>
#include<algorithm>
#include <iostream>
//algorithm sorting functions, using prebuilt min heap + pop_heap fn
struct CDijkstraPathRouter::SImplementation {
    using TEdge = std::pair<double, TVertexID>; 
    struct SVertex {
        std::any DTag;
        std::vector <TEdge> DEdges;
    };

    std::vector <SVertex> DVertices; 

    std::size_t VertexCount() const noexcept{
        return DVertices.size();
    }
    TVertexID AddVertex(std::any tag) noexcept {
        TVertexID NewVertexID = DVertices.size();//push vertex to DVertices
        DVertices.push_back({tag, {}});
        return NewVertexID;
    }
    std::any GetVertexTag(TVertexID id) const noexcept {
        if(id < DVertices.size()) {//vertex tag in range
            return DVertices[id].DTag;
        }
        return std::any();//tag of any type
    }
    bool AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept {
        if((src < DVertices.size()) && (dest < DVertices.size()))  { //check for negative edge weight, check if IDs are in range of DVertices
            //std::cout<<"edge added: "<<weight<<" "<<src<< " to "<<dest <<std::endl;
            DVertices[src].DEdges.push_back(std::make_pair(weight, dest));//opposite direction edge
            if(bidir == true) { //add directional edge in oppposite dir
                DVertices[dest].DEdges.push_back(std::make_pair(weight, src));
                //std::cout<<"edge added: "<<weight<<" "<<dest<< " to "<<src<<std::endl;
            }
            return true;
        }
        std::cout<<"edge failed"<<std::endl;
        return false;
    }
    bool Precompute(std::chrono::steady_clock::time_point deadline) noexcept {
        return true;
    }

    double FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept {
        std::vector < TVertexID > PendingVertices;
        std::vector < double > Distances(DVertices.size(), CPathRouter::NoPathExists);
        std::vector < TVertexID  > Previous(DVertices.size(), CPathRouter::InvalidVertexID);
        if(DVertices.size()== 0) {//empty map
            std::cout<<"why no vertices lil bro"<<std::endl;
            return NoPathExists;
        }
        //use a lambda to compare the actual distances instead of the id numbers
        //want them sorted by distances, so that the top of the heap is the closest distance possible, and opposite swapping for the longest
        auto VertexCompare = [&Distances](TVertexID left, TVertexID right) {return Distances[left] > Distances[right];}; //use inequality to populate the min-heap

        Distances[src] = 0.0; //set distances = to 0
        PendingVertices.push_back(src);//start with src

        while(!PendingVertices.empty()) {
            auto CurrentID = PendingVertices.front();
            if (CurrentID > DVertices.size()) {
                // Handle the out-of-bounds access here
                std::cout<<"ERROR currentID out of range"<<std::endl;
                return -1;
            }
            if(CurrentID == dest) {break;} //to skip if we are at the dest
            //std::cout<<CurrentID<<std::endl;
            std::pop_heap(PendingVertices.begin(), PendingVertices.end());
            PendingVertices.pop_back(); //will get current id off the top of the heap, removes the shortest distance from pendingvertices
            if(DVertices[CurrentID].DEdges.size() == 0){
                return NoPathExists;
            }
            for(auto Edge : DVertices[CurrentID].DEdges) { //go through edges
                auto EdgeWeight = Edge.first;
                auto DestID = Edge.second;
                if (CurrentID > DVertices.size() || DestID > DVertices.size()) {
                    // Handle the out-of-bounds access here
                    std::cout<<"ERROR DestID out of range"<<std::endl;
                    return -1;
                }
                auto TotalDistance = Distances[CurrentID] + EdgeWeight;
                if(TotalDistance < Distances[DestID]) { //check if distances are in the range
                    if(CPathRouter::NoPathExists == Distances[DestID]) {
                        PendingVertices.push_back(DestID);
                    }
                    Distances[DestID] = TotalDistance;
                    Previous[DestID] = CurrentID;
                }
            }
            //re heapify using vertexcompare to create min-heap
            std::make_heap(PendingVertices.begin(), PendingVertices.end(), VertexCompare);
        }
        if(CPathRouter::NoPathExists == Distances[dest]) {
            return CPathRouter::NoPathExists;
        }
        double PathDistance = Distances[dest];
        path.clear();
        path.push_back(dest);
        do {//iterate and reassign previous/dest
            dest = Previous[dest];
            path.push_back(dest);
        } while(dest != src);
        std::reverse(path.begin(), path.end());//reverse the travelled path
        return PathDistance;
    }
};

//defer all implementations
CDijkstraPathRouter::CDijkstraPathRouter() {
    DImplementation = std::make_unique<SImplementation>();
}
CDijkstraPathRouter::~CDijkstraPathRouter() {

}

std::size_t CDijkstraPathRouter::VertexCount() const noexcept {
    return DImplementation->VertexCount();
}

CDijkstraPathRouter::TVertexID CDijkstraPathRouter::AddVertex(std::any tag) noexcept {
    return DImplementation->AddVertex(tag);
}
std::any CDijkstraPathRouter::GetVertexTag(TVertexID id) const noexcept {
    return DImplementation->GetVertexTag(id);
}

bool CDijkstraPathRouter::AddEdge(TVertexID src, TVertexID dest, double weight, bool bidir) noexcept {
    return DImplementation->AddEdge(src, dest, weight, bidir);
}

bool CDijkstraPathRouter::Precompute(std::chrono::steady_clock::time_point deadline) noexcept {
    return DImplementation->Precompute(deadline);
}

double CDijkstraPathRouter::FindShortestPath(TVertexID src, TVertexID dest, std::vector<TVertexID> &path) noexcept {
    return DImplementation->FindShortestPath(src, dest, path);
}