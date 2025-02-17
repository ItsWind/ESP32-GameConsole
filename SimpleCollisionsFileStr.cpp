#include "SimpleCollisionsFileStr.h"

const char * SIMPLE_COLLISIONS_FILE_STR = R""""(

local this = {}

local boxes = {}

local function checkForCollision(box1, box2)
  -- Check if box1 and box2 are colliding
  if box1.x < box2.x + box2.w and
    box1.x + box1.w > box2.x and
    box1.y < box2.y + box2.h and
    box1.y + box1.h > box2.y then
    return true  -- Collision detected
  else
    return false -- No collision
  end
end

local function slideCollision(box1, box2)
  -- Check if box1 is colliding with box2
  if checkForCollision(box1, box2) then
    local overlapX = 0
    local overlapY = 0

    -- Check horizontal overlap
    if box1.x + box1.w > box2.x and box1.x < box2.x then
      overlapX = box2.x - (box1.x + box1.w)
    elseif box1.x < box2.x + box2.w and box1.x + box1.w > box2.x + box2.w then
      overlapX = (box2.x + box2.w) - box1.x
    end

    -- Check vertical overlap
    if box1.y + box1.h > box2.y and box1.y < box2.y then
      overlapY = box2.y - (box1.y + box1.h)
    elseif box1.y < box2.y + box2.h and box1.y + box1.h > box2.y + box2.h then
      overlapY = (box2.y + box2.h) - box1.y
    end

    -- one at a time
    if overlapX ~= 0 and overlapY ~= 0 then
      if math.abs(overlapX) < math.abs(overlapY) then
        overlapY = 0
      else
        overlapX = 0
      end
    end

    if overlapX ~= 0 then
      if box1.lockX ~= true and box2.lockX ~= true then
        local halfOverlap = overlapX / 2
        box1.x = box1.x + halfOverlap
        box2.x = box2.x - halfOverlap
      elseif box1.lockX ~= true then
        box1.x = box1.x + overlapX
      elseif box2.lockX ~= true then
        box2.x = box2.x - overlapX
      end
    elseif overlapY ~= 0 then
      if box1.lockY ~= true and box2.lockY ~= true then
        local halfOverlap = overlapY / 2
        box1.y = box1.y + halfOverlap
        box2.y = box2.y - halfOverlap
      elseif box1.lockY ~= true then
        box1.y = box1.y + overlapY
      elseif box2.lockX ~= true then
        box2.y = box2.y - overlapY
      end
    end
    return true
  end
  return false

  -- Return the adjusted positions
  --return box1.x + overlapX, box1.y + overlapY
end

function this.AddBox(parent, x, y, w, h)
  if parent.box ~= nil then return end

  if x == nil then x = 0 end
  if y == nil then y = 0 end
  if w == nil then w = 5 end
  if h == nil then h = 5 end

  local t = {
    x = x,
    y = y,
    w = w,
    h = h,
    parent = parent
  }

  parent.box = t
  boxes[#boxes + 1] = t
end

function this.RemoveBox(parent)
  if parent == nil then return end

  for i=1, #boxes do
    if boxes[i].parent == parent then
      table.remove(boxes, i)
      parent.box = nil
      return
    end
  end
end

function this.LockBox(parent, lockX, lockY)
  parent.box.lockX = lockX
  parent.box.lockY = lockY
end

function this.SlideBox(parent, modX, modY)
  local cols = {}
  parent.box.x = parent.box.x + modX
  parent.box.y = parent.box.y + modY

  for i=1, #boxes do
    if boxes[i].parent ~= parent then
      if slideCollision(parent.box, boxes[i]) then
        cols[#cols + 1] = boxes[i]
      end
    end
  end

  return cols
end

return this

)"""";