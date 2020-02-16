#include <vector>
#include <iostream>
#include <fstream>

struct Endpoint
{
    size_t idx;
    int datacenter_latency;
    std::vector<std::pair<int, int>> video_request;
};

struct Cache
{
    int capacity;
    std::vector<std::pair<Endpoint*, int>> endpoint_latency;
    std::vector<int> video_idx;
};

struct DataCenter
{
    /* data */
    std::vector<Endpoint *> endpoints;
    std::vector<Cache> caches;
    std::vector<int> video_size;

    /*processing*/
    void Arrange(void)
    {
        
    }
};

int main()
{
    DataCenter data_center;
    int V, E, R, C, X;
    std::ifstream fin("input_1");
    fin >> V >> E >> R >> C >> X;

    data_center.video_size.resize(V);
    data_center.caches.resize(C);
    data_center.endpoints.resize(E);

    //fill vodeo size
    for(int i = 0; i < V; ++i)
        fin >> data_center.video_size[i];

    //fill endpoint
    for(int i = 0; i < E; ++i)
    {
        int cache_num;
        Endpoint *ep = new Endpoint;
        ep->idx = i;
        data_center.endpoints[i] = ep;
        fin >> ep->datacenter_latency >> cache_num;

        while(cache_num--)
        {
            int cache_idx, latency;
            fin >> cache_idx >> latency;
            data_center.caches[cache_idx].capacity = X;
            data_center.caches[cache_idx].endpoint_latency.push_back({ep, latency});

        }
    }

    //fill request
    while(R--)
    {
        int video_idx, ep_idx, req;
        fin >> video_idx >> ep_idx >> req;
        data_center.endpoints[ep_idx]->video_request.push_back({video_idx, req});
    }

    data_center.Arrange();
// int i = 0;
//     for(auto ep: data_center.endpoints)
//     {
//         std::cout << "ep" << i << std::endl;
//         std::cout << ep->datacenter_latency << std::endl;
//         for(auto vr: ep->video_request)
//             std::cout << vr.first << " " << vr.second << std::endl;
//         i++;
//     }
// int i = 0;
//     for(auto cache: data_center.caches)
//     {
//         std::cout << "cache" << i << std::endl;
//         std::cout << cache.capacity << std::endl;
//         for(auto vr: cache.endpoint_latency)
//             std::cout << vr.first->idx << " " << vr.second << std::endl;
//         i++;
//     }
}
