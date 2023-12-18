#pragma once

#include <vector>
#include <numeric>
#include <cmath>

namespace Engine
{
    template<class PointT, unsigned DIM>
    class KDTree {
    public:
        /** @brief The constructors.
        */
        KDTree() : root_(nullptr) {};

        KDTree(const std::vector<PointT> &points) : root_(nullptr) { build(points); }

        /** @brief The destructor.
        */
        ~KDTree() { clear(); }

        /** @brief Re-builds k-d tree.
        */
        void build(const std::vector<PointT> &points) {
            clear();

            points_ = points;

            std::vector<int> indices(points.size());
            std::iota(std::begin(indices), std::end(indices), 0);

            root_ = buildRecursive(indices.data(), (int) points.size(), 0);
        }

        /** @brief Clears k-d tree.
        */
        void clear() {
            clearRecursive(root_);
            root_ = nullptr;
            points_.clear();
        }

        /** @brief Validates k-d tree.
        */
        bool validate() const {
            try {
                validateRecursive(root_, 0);
            }
            catch (const Exception &) {
                return false;
            }

            return true;
        }

        /** @brief Searches the nearest neighbor.
        */
        int nnSearch(const PointT &query, double *minDist = nullptr) const {
            int guess;
            double _minDist = std::numeric_limits<double>::max();

            nnSearchRecursive(query, root_, &guess, &_minDist);

            if (minDist)
                *minDist = _minDist;

            return guess;
        }


        /** @brief Searches neighbors within radius.
        */
        std::vector<int> radiusSearch(const PointT &query, double radius) const {
            std::vector<int> indices;
            radiusSearchRecursive(query, root_, indices, radius);
            return indices;
        }

    private:

        /** @brief k-d tree node.
        */
        struct Node {
            int idx;       //!< index to the original point
            Node *next[2]; //!< pointers to the child nodes
            int axis;      //!< dimension's axis

            Node() : idx(-1), axis(-1) { next[0] = next[1] = nullptr; }
        };

        /** @brief k-d tree exception.
        */
        class Exception : public std::exception {
            using std::exception::exception;
        };


        /** @brief Builds k-d tree recursively.
        */
        Node *buildRecursive(int *indices, int npoints, int depth) {
            if (npoints <= 0)
                return nullptr;

            const int axis = depth % DIM;
            int mid = (npoints - 1) / 2;
            Node *node = new Node();

            node->axis = axis;
            /*
            if (npoints > 100) {

                std::vector<int> subsetIndices;
                for (auto i = 0; i < npoints; i += 10) subsetIndices.push_back(indices[i]);

                int* sIndices = subsetIndices.data();
                const int sNpoints = (int)subsetIndices.size();
                const int sMid = (sNpoints - 1) / 2;
                std::nth_element(sIndices, sIndices + sMid, sIndices + sNpoints, [&](int lhs, int rhs)
                    {
                        return points_[lhs][axis] < points_[rhs][axis];
                    });

                int median = *(sIndices + sMid);
                subsetIndices.resize(npoints);
                subsetIndices.assign(indices, indices + npoints);
                int left = 0;
                int right = 0;
                for (int i = 0; i < npoints; i++)
                {
                    int idx = subsetIndices[i];
                    if (points_[idx][axis] < points_[median][axis]) {
                        indices[left] = idx;
                        left++;
                    }
                    else {
                        indices[npoints - 1 - right] = idx;
                        right++;
                    }
                }
                mid = left;
                subsetIndices.clear();
            }
            else*/

            std::nth_element(indices, indices + mid, indices + npoints, [&](int lhs, int rhs) {
                return points_[lhs][axis] < points_[rhs][axis];
            });


            node->idx = indices[mid];
            node->next[0] = buildRecursive(indices, mid, depth + 1);
            node->next[1] = buildRecursive(indices + mid + 1, npoints - mid - 1, depth + 1);

            return node;
        }

        /** @brief Clears k-d tree recursively.
        */
        void clearRecursive(Node *node) {
            if (node == nullptr)
                return;

            if (node->next[0])
                clearRecursive(node->next[0]);

            if (node->next[1])
                clearRecursive(node->next[1]);

            delete node;
        }

        /** @brief Validates k-d tree recursively.
        */
        void validateRecursive(const Node *node, int depth) const {
            if (node == nullptr)
                return;

            const int axis = node->axis;
            const Node *node0 = node->next[0];
            const Node *node1 = node->next[1];

            if (node0 && node1) {
                if (points_[node->idx][axis] < points_[node0->idx][axis])
                    throw Exception();

                if (points_[node->idx][axis] > points_[node1->idx][axis])
                    throw Exception();
            }

            if (node0)
                validateRecursive(node0, depth + 1);

            if (node1)
                validateRecursive(node1, depth + 1);
        }

        static double distance(const PointT &p, const PointT &q) {
            double dist = 0;
            for (size_t i = 0; i < DIM; i++)
                dist += (p[i] - q[i]) * (p[i] - q[i]);
            return sqrt(dist);
        }

        /** @brief Searches the nearest neighbor recursively.
        */
        void nnSearchRecursive(const PointT &query, const Node *node, int *guess, double *minDist) const {
            if (node == nullptr)
                return;

            const PointT &train = points_[node->idx];

            const double dist = distance(query, train);
            if (dist < *minDist) {
                *minDist = dist;
                *guess = node->idx;
            }

            const int axis = node->axis;
            const int dir = query[axis] < train[axis] ? 0 : 1;
            nnSearchRecursive(query, node->next[dir], guess, minDist);

            const double diff = fabs(query[axis] - train[axis]);
            if (diff < *minDist)
                nnSearchRecursive(query, node->next[!dir], guess, minDist);
        }


        /** @brief Searches neighbors within radius.
        */
        void
        radiusSearchRecursive(const PointT &query, const Node *node, std::vector<int> &indices, double radius) const {
            if (node == nullptr)
                return;

            const PointT &train = points_[node->idx];

            const double dist = distance(query, train);
            if (dist < radius)
                indices.push_back(node->idx);

            const int axis = node->axis;
            const int dir = query[axis] < train[axis] ? 0 : 1;
            radiusSearchRecursive(query, node->next[dir], indices, radius);

            const double diff = fabs(query[axis] - train[axis]);
            if (diff < radius)
                radiusSearchRecursive(query, node->next[!dir], indices, radius);
        }

        Node *root_;                 //!< root node
        std::vector<PointT> points_; //!< points
    };
}
