//
// Created by Tiago on 16/11/2019.
//

#include <algorithm>
#include "AfterSalesService.h"

using namespace std;

int AfterSalesService::tableOrdinal = 0;

AfterSalesService::AfterSalesService(int toWrapQS) : toWrapQueueSize(toWrapQS) {

}

vector<Article*> AfterSalesService::getPresentsTable() const {
    return presentsTable;
}

queue<Article*> AfterSalesService::getToWrap() const {
    return toWrap;
}

queue<Article*> AfterSalesService::getToDeliver() const {
    return toDeliver;
}

/**
 *  Place presents on the presents table, assigning them a relative present number and a sequential number.
 *  Assumes all given presents are from the same client.
 */
void AfterSalesService::dropPresentsOnTable(vector<Article*> presents) {
    for(vector<Article*>::size_type i = 0; i<presents.size(); i++) {
        presents[i]->setPresentNumber(i+1);
        presents[i]->setArrivalNumber(++tableOrdinal);
        presentsTable.push_back(presents[i]);
    }
}

/**
 * Remove from the table all articles of a given client.
 */
vector<Article*> AfterSalesService::pickPresentsFromTable(long client) {
    vector<Article*> clientPresents;
    auto presentsTableIterator = presentsTable.begin();

    while (presentsTableIterator != presentsTable.end())
    {
        if ((*presentsTableIterator)->getClient() == client)
        {
            clientPresents.push_back(*presentsTableIterator);
            presentsTableIterator = presentsTable.erase(presentsTableIterator);
        }
        else
        {
            presentsTableIterator++;
        }
    }

    return clientPresents;
}

bool lessThan(Article* &a1, Article* &a2)
{
    if (a1->getPresentNumber() + 2 < a2->getPresentNumber())
        return a1->getPresentNumber() < a2->getPresentNumber();
    return a1->getArrivalNumber() < a2->getArrivalNumber();
}


/**
 * Sort the first 10 articles in the presents table:
 *   (i) give priority to articles that are NOT to be delivered at home (client is waiting);
 *  (ii) if two articles have (relative) present numbers differing more than 2,
 *       the one with lower present number should have priority (clients should not wait too much);
 * (iii) else, maintain article order in the table.
 */
void AfterSalesService::sortArticles() {
    vector<Article*> deliverHomeVector;
    vector<Article*> notDeliverHomeVector;
    vector<Article*> toAlter;
    auto vectorIterator = presentsTable.begin();

    while (vectorIterator != presentsTable.begin()+10)
    {
        if ((*vectorIterator)->getDeliverHome())
        {
            deliverHomeVector.push_back(*vectorIterator);
            vectorIterator++;
        }
        else
        {
            notDeliverHomeVector.push_back(*vectorIterator);
            vectorIterator++;
        }
    }

    //std::sort(deliverHomeVector.begin(), deliverHomeVector.end(), ::lessThan);
    std::sort(notDeliverHomeVector.begin(), notDeliverHomeVector.end(), ::lessThan);
    toAlter.insert(toAlter.begin(), notDeliverHomeVector.begin(), notDeliverHomeVector.end());
    toAlter.insert(toAlter.end(), deliverHomeVector.begin(), deliverHomeVector.end());

    for (int i = 0; i < 10; i++)
    {
        presentsTable[i] = toAlter[i];
    }

    /*
    for (int i = 0; i < notDeliverHomeVector.size(); i++)
    {
        presentsTable[i] = notDeliverHomeVector[i];
    }

    for (int i = notDeliverHomeVector.size(); i < deliverHomeVector.size() + notDeliverHomeVector.size(); i++)
    {
        presentsTable[i] = deliverHomeVector[i-notDeliverHomeVector.size()];
    }
    */
}

/**
 * Move the first presents in the table to the queue of presents to wrap, taking into account the size of the queue.
 */
void AfterSalesService::enqueueArticles() {
    int max = toWrapQueueSize - toWrap.size();

    if (presentsTable.size() < max)
    {
        max = presentsTable.size();
    }

    for (int i = 0; i < max; i++)
    {
        toWrap.push(presentsTable[i]);
    }
    presentsTable.erase(presentsTable.begin(), presentsTable.begin() + max);

}

/**
 * Process the first present to wrap. If it is an article to deliver home, the article is placed in the toDeliver queue.
 */
Article* AfterSalesService::wrapNext() {
    if (toWrap.size() == 0)
    {
        return NULL;
    }
    if (toWrap.front()->getDeliverHome())
    {
        toDeliver.push(toWrap.front());
        toWrap.pop();
    }
    else
    {
        auto temp = toWrap.front();
        toWrap.pop();
        return temp;
    }
    return NULL;

}

