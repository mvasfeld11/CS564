#include "join.hpp"

#include <array>
#include <cstdint>
#include <vector>
#include <unordered_map>

JoinAlgorithm getJoinAlgorithm() {
  // TODO: Return your chosen algorithm.
  return JOIN_ALGORITHM_BNLJ;
  // return JOIN_ALGORITHM_SMJ;
  // return JOIN_ALGORITHM_HJ;
  // throw std::runtime_error("not implemented: getJoinAlgorithm");
};

int join(File &file, int numPagesR, int numPagesS, char *buffer, int numFrames) {
    // Set the starting indices for relations R, S, and the output relation
    int rStartIndex = 0;
    int sStartIndex = rStartIndex + numPagesR;
    int outputStartIndex = sStartIndex + numPagesS;

    // Calculate the block size and the number of blocks required for relations R and S
    int rBlockSize = numFrames - 1;
    int rBlockCount = (numPagesR + rBlockSize - 1) / rBlockSize;
    int sBlockCount = numPagesS;

    // Initialize a vector to store the output tuples
    std::vector<Tuple> outputTuples;
    int outputPageIndex = 0;

    // Iterate over each block of relation R
    for (int rBlockIdx = 0; rBlockIdx < rBlockCount; ++rBlockIdx) {
        int rBlockStartPage = rStartIndex + rBlockIdx * rBlockSize;
        int rBlockPageCount = std::min(rBlockSize, numPagesR - rBlockIdx * rBlockSize);
        file.read(buffer, rBlockStartPage, rBlockPageCount);

        // Iterate over each block of relation S
        for (int sBlockIdx = 0; sBlockIdx < sBlockCount; ++sBlockIdx) {
            int sBlockStartPage = sStartIndex + sBlockIdx;
            file.read(buffer + rBlockSize * PAGE_SIZE, sBlockStartPage, 1);

            std::unordered_map<int32_t, std::vector<int>> matchingTuplesS;
            for (int j = 0; j < 512; ++j) {
                Tuple &tupleS = reinterpret_cast<Tuple *>(buffer + rBlockSize * PAGE_SIZE)[j];
                matchingTuplesS[tupleS.first].push_back(j);
            }

            for (int i = 0; i < rBlockPageCount * 512; ++i) {
                Tuple &tupleR = reinterpret_cast<Tuple *>(buffer)[i];

                if (i > 0 && tupleR.first == reinterpret_cast<Tuple *>(buffer)[i - 1].first) {
                    continue;
                }
                auto found = matchingTuplesS.find(tupleR.first);
                if (found != matchingTuplesS.end()) {
                    for (int j : found->second) {
                        Tuple &tupleS = reinterpret_cast<Tuple *>(buffer + rBlockSize * PAGE_SIZE)[j];
                        outputTuples.emplace_back(tupleR.second, tupleS.second);
                        if (outputTuples.size() == 512) {
                            file.write(outputTuples.data(), outputStartIndex + outputPageIndex);
                            outputPageIndex++;
                            outputTuples.clear();
                        }
                    }
                }
            }
        }
    }

    // Write any remaining tuples in the output vector to the file
    if (!outputTuples.empty()) {
        file.write(outputTuples.data(), outputStartIndex + outputPageIndex);
    }
    // Return the total number of output tuples
    return outputPageIndex * 512 + outputTuples.size();
}







