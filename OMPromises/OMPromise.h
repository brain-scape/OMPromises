//
// OMPromise.h
// OMPromises
//
// Copyright (C) 2013 Oliver Mader
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

/** Possible states of an @p OMPromise.
 */
typedef enum OMPromiseState {
    OMPromiseStateUnfulfilled = 0,
    OMPromiseStateFailed = 1,
    OMPromiseStateFulfilled = 2
} OMPromiseState;


/** Proxies the outcome of a deferred.
 */
@interface OMPromise : NSObject

///---------------------------------------------------------------------------------------
/// @name Current State
///---------------------------------------------------------------------------------------

/** Current state.

 May only change from @p OMPromiseStateUnfulfilled to either @p OMPromiseStateFailed or
 @p OMPRomiseStateFulfilled.
 */
@property(assign, readonly) OMPromiseState state;

/** Maybe the promised result.
 
 Contains the result in case the promise has been fulfilled.
 */
@property(readonly) id result;

/** Maybe an error.
 
 Contains the reason in case the promise failed.
 */
@property(readonly) NSError *error;

/** Progress of the underlying workload.
 
 Describes the progress of the underyling workload as a floating point number in range
 [0, 1]. It only increases.
 */
@property(readonly) NSNumber *progress;

///---------------------------------------------------------------------------------------
/// @name Return
///---------------------------------------------------------------------------------------

/** Create a fulfilled promise.
 
 Simply wraps the supplied value inside a fulfiled promise.
 
 @param result The value to fulfil the promise.
 @return A fulfiled promise.
 */
+ (OMPromise *)promiseWithResult:(id)result;

/** Create a promise which gets fulfilled after a certain delay.

 After a certain amount of time the promise gets fulfilled using the supplied value.
 
 @param result The value to fulfil the promise.
 @param delay Time span to wait before fulfilling the promise.
 @return A promise that will get fulfilled.
 @see promiseWithResult:
 */
+ (OMPromise *)promiseWithResult:(id)result after:(NSTimeInterval)delay;

/** Create a failed promise.
 
 @param error Reason why the promise failed.
 @return A failed promise.
 */
+ (OMPromise *)promiseWithError:(NSError *)error;

/** Create a promise which fails after a certain delay.

 After a certain amount of time the promise fails using the supplied error.
 
 @param error Reason why the promise failed.
 @param delay Time span to wait before the promise fails.
 @return A promise that will fail.
 @see promiseWithError:
 */
+ (OMPromise *)promiseWithError:(NSError *)error after:(NSTimeInterval)delay;

///---------------------------------------------------------------------------------------
/// @name Bind
///---------------------------------------------------------------------------------------

/** Create a new promise by binding the fulfilled result to another promise.

 The supplied block gets called in case the promise gets fulfilled. The block can return
 a simple value or another block, in both cases the promise returned by this method
 is bound to the result of the block.

 If the promise fails, the chain is short-circuited and the resulting promise fails too.

 @param thenHandler Block to be called once the promise gets fulfilled.
 @return A new promise.
 @see rescue:
 */
- (OMPromise *)then:(id (^)(id result))thenHandler;

/** Create a new promise by binding the error reason to another promise.

 Similiar to then:, but the supplied block is called in case the promise fails. from
 which point on it behaves like then:. If the promise gets fulfilled the step is skipped.

 @param rescueHandler Block to be called once the promise failed.
 @return A new promise.
 @see then:
 */
- (OMPromise *)rescue:(id (^)(NSError *error))rescueHandler;

///---------------------------------------------------------------------------------------
/// @name Callbacks
///---------------------------------------------------------------------------------------

/** Register a block to be called when the promise gets fulfilled.

 @param fulfilHandler Block to be called.
 @return The promise itself.
 */
- (OMPromise *)fulfilled:(void (^)(id result))fulfilHandler;

/** Register a block to be called when the promise fails.

 @param failHandler Block to be called.
 @return The promise itself.
 */
- (OMPromise *)failed:(void (^)(NSError *))failHandler;

/** Register a block to be called when the promise progresses.

 @param progressHandler Block to be called.
 @return The promise itself.
 */
- (OMPromise *)progressed:(void (^)(NSNumber *))progressHandler;

///---------------------------------------------------------------------------------------
/// @name Combinators
///---------------------------------------------------------------------------------------

/** Create a promise chain as if you would do multiple then binds.

 It also respects the progress of each chain step by assuming an equal distribution
 of work over all items, such that it also updates the progress with respect to
 each individual step.

 @param thenHandlers Sequence of then: handler blocks.
 @param initial Initial result supplied to the first then: handler block.
 @return A new promise describing the whole chain.
 */
+ (OMPromise *)chain:(NSArray *)thenHandlers initial:(id)result;

/** Race for the first fulfilled promise in parallel.

 The new returned promise gets fulfilled if any of the supplied promises does.
 If no promise gets fulfilled, the returned promise fails.

 The progress aligns to the mostly progressed promise.

 @param promises A sequence of promises.
 @return A new promise.
 */
+ (OMPromise *)any:(NSArray *)promises;

/** Wait for all promises to get fulfilled.

 In case that all supplied promises get fulfilled, the promise itself returns
 an array containing all results for the supplied promises while respecting the
 correct order. Nil has been replaced by [NSNull null]. If any promise fails,
 the returned promise fails also.

 Similiar to chain: the workload of each promise is considered equal to
 determine the overall progress.

 @param promises A sequence of promises.
 @return A new promise.
 */
+ (OMPromise *)all:(NSArray *)promises;

@end
