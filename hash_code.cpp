#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

struct Video
{
    int idx;
    int request;
    int size;

};

bool operator<(const Video &l, const Video &r)
{
    return l.request/l.size < r.request/r.size;
}

struct Endpoint
{
    size_t idx;
    int datacenter_latency;
    std::vector<Video> video_request;
    std::vector<bool> closed;
};

struct Cache
{
    int capacity;
    std::priority_queue<std::pair<int, Endpoint*>> endpoint_latency; //<lat, ep>
    std::vector<int> video_idx;
};

struct DataCenter
{
    /* data */
    std::vector<Endpoint *> endpoints;
    std::vector<Cache> caches;
    std::vector<int> video_size;

    void Output(void)
    {
        int used_cash = 0;
        for(auto &cache: caches)
            if (cache.video_idx.size() > 0) used_cash++;
        std::ofstream fout("output");
        fout << used_cash << "\n";
        for(int i = 0; i < caches.size(); ++i)
        {
            std::cout << "caches " << i << ": " << std::endl;
            if (caches[i].video_idx.size())
            {
                std::cout << "video idx: ";
                fout << i << " ";
                for(auto j = 0; j < caches[i].video_idx.size(); ++j)
                {
                    if (j != 0)
                        fout << " ";
                    fout << caches[i].video_idx[j];
                    std::cout << caches[i].video_idx[j] << " ";
                }
                std::cout << std::endl;
                fout << "\n";
            }
        }

    }

    void TestData(void)
    {
        int i = 0;
        for(auto ep: endpoints)
        {
            std::cout << "ep " << i << ":" << std::endl;
            std::cout << "dc_latency: " << ep->datacenter_latency << std::endl;
            std::cout << "video:" << std::endl;
            for(auto & video: ep->video_request)
            {
                std::cout << video.idx << ": " << video.request/video.size << std::endl;
            }
            i++;
        }
    }

    void SortEpVideo(void)
    {
        for(auto & ep: endpoints)
            std::sort(ep->video_request.begin(), ep->video_request.end());
    }

    /*processing*/
    void Arrange(void)
    {
        //fill caches
        for(auto & cache: caches) //for all caches
        {
            while(!cache.endpoint_latency.empty()) //till we have ep connected to the particular cache
            {
                auto ep = cache.endpoint_latency.top().second;  //get ep with less latency
                for(int i = 0; i < ep->video_request.size(); ++i)   //go though all videos starting from the most profitamle
                {
                    if (ep->closed[i])  //if video was already cached, continue
                        continue;
                    if (cache.capacity >= ep->video_request[i].size)    //if cache has enough space for a particular video
                    {
                        if (std::find(std::begin(cache.video_idx), std::end(cache.video_idx), ep->video_request[i].idx) == std::end(cache.video_idx)) //and not stored in cache yet
                        {
                            cache.capacity -= ep->video_request[i].size;    //update capacity
                            cache.video_idx.push_back(ep->video_request[i].idx);    //save video index
                        }
                        ep->closed[i] = true;   // mark video as stored in cache
                    }
                }
                cache.endpoint_latency.pop();   //del pointer to ep from cache's ep list
            }
        }
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
            data_center.caches[cache_idx].endpoint_latency.push({latency, ep});

        }
    }

    //fill request
    while(R--)
    {
        int video_idx, ep_idx, req;
        fin >> video_idx >> ep_idx >> req;
        data_center.endpoints[ep_idx]->video_request.push_back({video_idx, req, data_center.video_size[video_idx]});
        data_center.endpoints[ep_idx]->closed.push_back(false);
    }

    data_center.SortEpVideo();
    // data_center.TestData();
    data_center.Arrange();
    data_center.Output();
}
