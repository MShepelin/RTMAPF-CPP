#pragma once

#include <vector>
#include "search_types.h"
#include <iostream>
#include <optional>
#include <unordered_map>

class Space
{
public:
  virtual Access GetAccess(Point point) const = 0;
  virtual void SetAccess(Point point, Access newAccess) = 0;
  virtual bool Contains(Point point) const = 0;

  virtual ~Space() {};
};

class RawSpace : public Space
{
private:
  std::vector<Access> grid;
  uint32_t width;
  uint32_t height;

private:
  inline size_t PointToIndex(Point& point) const;

public:
  RawSpace() = delete;
  RawSpace(uint32_t inWidth, uint32_t inHeight);

  Access GetAccess(Point point) const override;
  void SetAccess(Point point, Access newAccess) override;

  uint32_t GetWidth() const;
  uint32_t GetHeight() const;

  bool Contains(Point point) const override;
};

class SpaceReader
{
private:
  std::unordered_map<char, Access> symbolToAccess;

  inline bool CheckHogFileStart(std::istream& file, uint32_t& width, uint32_t& height);

public:
  SpaceReader();

  std::optional<RawSpace> FromHogFormat(std::istream& file);
};
