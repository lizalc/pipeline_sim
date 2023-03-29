// Donavan Lance (dlance)
// ECE 463 Fall 2018
// Project 3 - Superscalar Pipeline Simulator

#include "instruction_metadata.h"
#include "pipeline_names.h"
#include <ostream>

InstructionMetadata::InstructionMetadata(int sequenceNum, int opType, int destReg,
                                         int srcReg1, int srcReg2)
    : sequenceNum{sequenceNum},
      opType{opType},
      destReg{destReg},
      srcReg1{srcReg1},
      srcReg2{srcReg2},
      stageCycles{
          {PipelineStage::Fetch, std::make_pair(0, 0)},
          {PipelineStage::Decode, std::make_pair(0, 0)},
          {PipelineStage::Rename, std::make_pair(0, 0)},
          {PipelineStage::RegisterRead, std::make_pair(0, 0)},
          {PipelineStage::Dispatch, std::make_pair(0, 0)},
          {PipelineStage::Issue, std::make_pair(0, 0)},
          {PipelineStage::Execute, std::make_pair(0, 0)},
          {PipelineStage::Writeback, std::make_pair(0, 0)},
          {PipelineStage::Retire, std::make_pair(0, 0)},
      }
{}

void InstructionMetadata::initCycle(PipelineStage stage, unsigned long cycle)
{
	stageCycles[stage].first = cycle;
}

void InstructionMetadata::updateCycle(PipelineStage stage)
{
	++stageCycles[stage].second;
}

int InstructionMetadata::dest() const
{
	return destReg;
}

int InstructionMetadata::src1() const
{
	return srcReg1;
}

int InstructionMetadata::src2() const
{
	return srcReg2;
}

std::pair<int, int> InstructionMetadata::operator[](PipelineStage stage) const
{
	return stageCycles.at(stage);
}

std::ostream &operator<<(std::ostream &stream, const InstructionMetadata &data)
{
	stream << data.sequenceNum << ' ';
	stream << "fu{" << data.opType << "} ";
	stream << "src{" << data.srcReg1 << ',' << data.srcReg2 << "} ";
	stream << "dst{" << data.destReg << "} ";

	auto stageData = data[PipelineStage::Fetch];
	stream << "FE{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Decode];
	stream << "DE{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Rename];
	stream << "RN{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::RegisterRead];
	stream << "RR{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Dispatch];
	stream << "DI{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Issue];
	stream << "IS{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Execute];
	stream << "EX{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Writeback];
	stream << "WB{" << stageData.first << ',' << stageData.second << "} ";

	stageData = data[PipelineStage::Retire];
	stream << "RE{" << stageData.first << ',' << stageData.second << "} ";

	return stream;
}
