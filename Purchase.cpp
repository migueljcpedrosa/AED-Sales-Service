//
// Created by Tiago on 16/11/2019.
//

#include "Purchase.h"
#include <stack>

using namespace std;

Purchase::Purchase(long cli) : client (cli) {
}

long Purchase::getClient() const {
    return client;
}

list< stack<Article*> > Purchase::getBags() const {
    return bags;
}

/**
 * Create an Article associated with the client of this purchase.
 */
Article* Purchase::createArticle(long barCode, bool present, bool deliverHome) {
    Article* articleCreate = new Article(client, barCode);
    articleCreate->setPresent(present);
    articleCreate->setDeliverHome(deliverHome);
    return articleCreate;
}

/**
 * Add an Article to the bags of this purchase. A new bag is added when the last bag is full.
 */
void Purchase::putInBag(Article* article) {
    auto position = bags.end();
    position--;
    if (position->size() < Purchase::BAG_SIZE)
    {
        position->push(article);
    }
    else
    {
        stack<Article *> newBag;
        newBag.push(article);
        bags.push_back(newBag);
    }

}

/**
 * Pop from the bags all articles that are presents.
 * All other articles are pushed back into the bags where they were, maintaining order.
 */
vector<Article*> Purchase::popPresents() {
    vector<Article*> presentsRemoved;
    stack<Article *> auxStack;
    for (auto &bag : bags)
    {
        while(!bag.empty())
        {
            if(bag.top()->getPresent())
            {
                presentsRemoved.push_back(bag.top());
            }
            else
            {
                auxStack.push(bag.top());
            }
            bag.pop();
        }

        while(!auxStack.empty())
        {
            bag.push(auxStack.top());
            auxStack.pop();
        }
    }

    return presentsRemoved;
}

